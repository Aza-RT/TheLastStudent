extends Node2D
## Composition root: Godot owns input, scenes, audio and UI; C++ owns graphics/perception/AI.
const WorldScript = preload("res://scripts/world/world.gd")
const PlayerScript = preload("res://scripts/player/player.gd")
const ObjectScript = preload("res://scripts/world/interactable.gd")
const ShadeScript = preload("res://scripts/world/shade.gd")
const BackdropScript = preload("res://scripts/ui/menu_backdrop.gd")
const INK := Color("101e2b")
const TEXT := Color("dbe4d4")
const MUTED := Color("8da5a3")
const GOLD := Color("d2b77b")
var world: CampusWorld
var player: StudentPlayer
var camera: Camera2D
var shade: Node2D
var shade_ai: TLSShadeAI
var lighting: TLSLightingRenderer
var stats: TLSDebugStats
var objects: Array[Node2D] = []
var nearby: Node2D
var ui: Control
var hud: Control
var menu: Control
var modal: Control
var fallback: ColorRect
var fallback_material: ShaderMaterial
var objective_label: Label
var status_label: Label
var dialogue_label: Label
var prompt_label: Label
var debug_label: Label
var debug_panel: Panel
var modal_kind: String = ""
var in_menu: bool = true
var debug_enabled: bool = false
var raw_enabled: bool = true
var threat: float = 0.0
var effect_timer: float = 0.0
var mask_timer: float = 0.0
var clock: float = 0.0
var first_sighting: bool = false
var capture_flash: ColorRect
var inventory_refresh_pending: bool = false
var smoothed_fps: float = 60.0
var quitting: bool = false
var flash_color := Color("a32b35")

func _ready() -> void:
    get_tree().auto_accept_quit = false
    seed(2410)
    world = WorldScript.new()
    add_child(world)
    for path: String in ["block1", "block2", "block3", "corridors"]:
        world.add_child(load("res://scenes/world/" + path + ".tscn").instantiate())
    for definition: Dictionary in world.data.objects:
        var object: Node2D = ObjectScript.new()
        object.definition = definition
        var pos: Array = definition.pos
        object.position = Vector2(pos[0] * 32 + 16, pos[1] * 32 + 16)
        world.add_child(object)
        objects.append(object)
    player = PlayerScript.new()
    player.world = world
    player.position = Vector2(1200, 320)
    add_child(player)
    camera = Camera2D.new()
    camera.position = player.position
    camera.zoom = Vector2(1.65, 1.65)
    camera.position_smoothing_enabled = false
    add_child(camera)
    shade = ShadeScript.new()
    shade.position = Vector2(544, 544)
    add_child(shade)
    shade_ai = TLSShadeAI.new()
    setup_shade()
    stats = TLSDebugStats.new()
    lighting = TLSLightingRenderer.new()
    add_child(lighting)
    lighting.set_walls(world.edges)
    raw_enabled = bool(ProjectSettings.get_setting("tls/rendering/raw_opengl", true)) and DisplayServer.get_name() != "headless"
    if OS.get_cmdline_user_args().has("--fallback"): raw_enabled = false
    lighting.set_raw(raw_enabled)
    build_ui()
    DialogueManager.spoken.connect(on_spoken)
    GameState.escaped.connect(on_escaped)
    GameState.player_caught.connect(on_caught)
    Inventory.changed.connect(on_inventory_changed)
    show_main_menu()
    if OS.get_cmdline_user_args().has("--capture"):
        capture_run.call_deferred()
    if OS.get_cmdline_user_args().has("--benchmark"):
        benchmark_run.call_deferred()

func setup_shade() -> void:
    var points := PackedVector2Array()
    for point: Array in world.data.patrol: points.append(Vector2(point[0], point[1]))
    shade_ai.setup(Vector2(544, 544), points, world.edges)

func panel(parent: Node, rect: Rect2, color: Color = INK, border: Color = Color("384d55")) -> Panel:
    var node := Panel.new()
    node.position = rect.position
    node.size = rect.size
    var style := StyleBoxFlat.new()
    style.bg_color = color
    style.border_color = border
    style.set_border_width_all(1)
    style.set_corner_radius_all(3)
    node.add_theme_stylebox_override("panel", style)
    parent.add_child(node)
    return node

func label(parent: Node, text: String, rect: Rect2, font_size: int = 18, color: Color = TEXT) -> Label:
    var node := Label.new()
    node.text = text
    node.position = rect.position
    node.size = rect.size
    node.add_theme_font_size_override("font_size", font_size)
    node.add_theme_color_override("font_color", color)
    parent.add_child(node)
    return node

func button(parent: Node, text: String, rect: Rect2, callback: Callable, accent: bool = false) -> Button:
    var node := Button.new()
    node.text = text
    node.position = rect.position
    node.size = rect.size
    node.mouse_default_cursor_shape = Control.CURSOR_POINTING_HAND
    node.add_theme_font_size_override("font_size", 18)
    for state: String in ["normal", "hover", "pressed", "focus"]:
        var style := StyleBoxFlat.new()
        style.bg_color = Color("bfa975") if accent else Color("203540")
        if state == "hover": style.bg_color = Color("d8c48f") if accent else Color("304b55")
        style.border_color = GOLD if state == "focus" else Color("45616a")
        style.set_border_width_all(2 if state == "focus" else 1)
        style.set_corner_radius_all(3)
        node.add_theme_stylebox_override(state, style)
    node.add_theme_color_override("font_color", Color("13232c") if accent else TEXT)
    node.add_theme_color_override("font_hover_color", Color("13232c") if accent else Color.WHITE)
    node.pressed.connect(callback)
    parent.add_child(node)
    return node

func build_ui() -> void:
    var effects := CanvasLayer.new()
    effects.layer = 5
    add_child(effects)
    fallback = ColorRect.new()
    fallback.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
    fallback.mouse_filter = Control.MOUSE_FILTER_IGNORE
    fallback_material = ShaderMaterial.new()
    fallback_material.shader = preload("res://assets/ui/fallback.gdshader")
    fallback.material = fallback_material
    effects.add_child(fallback)
    var canvas := CanvasLayer.new()
    canvas.layer = 10
    add_child(canvas)
    ui = Control.new()
    ui.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
    ui.mouse_filter = Control.MOUSE_FILTER_IGNORE
    canvas.add_child(ui)
    hud = Control.new()
    hud.mouse_filter = Control.MOUSE_FILTER_IGNORE
    ui.add_child(hud)
    panel(hud, Rect2(24, 20, 858, 78), Color(0.035, 0.07, 0.11, 0.94))
    label(hud, "NIGHT SHIFT     /     AITU, FIRST FLOOR", Rect2(42, 28, 790, 22), 12, GOLD)
    objective_label = label(hud, "", Rect2(42, 53, 820, 32), 16)
    panel(hud, Rect2(900, 20, 356, 78), Color(0.035, 0.07, 0.11, 0.94))
    status_label = label(hud, "", Rect2(918, 30, 320, 58), 15)
    var bottom := panel(hud, Rect2(24, 608, 1232, 88), Color(0.035, 0.07, 0.11, 0.94))
    label(bottom, "INNER MONOLOGUE", Rect2(16, 6, 1100, 18), 10, GOLD)
    dialogue_label = label(bottom, "", Rect2(16, 27, 1198, 49), 17)
    dialogue_label.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
    panel(hud, Rect2(130, 548, 1020, 46), Color(0.035,0.07,0.11,0.94))
    prompt_label = label(hud, "", Rect2(140, 554, 1000, 32), 16, GOLD)
    prompt_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
    debug_panel = panel(hud, Rect2(24, 112, 690, 426), Color(0.02, 0.04, 0.07, 0.96))
    debug_label = label(debug_panel, "", Rect2(15, 10, 660, 410), 13)
    debug_panel.visible = false
    capture_flash = ColorRect.new()
    capture_flash.color = Color(0.6, 0.12, 0.14, 0)
    capture_flash.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
    capture_flash.mouse_filter = Control.MOUSE_FILTER_IGNORE
    ui.add_child(capture_flash)
    menu = Control.new()
    ui.add_child(menu)
    update_ui_regions()

func update_ui_regions() -> void:
    var rects := PackedFloat32Array()
    var regions: Array[Rect2] = [Rect2(24,20,858,78), Rect2(900,20,356,78), Rect2(24,608,1232,88), Rect2(130,548,1020,46)]
    if debug_enabled: regions.append(Rect2(24,112,690,426))
    for rect: Rect2 in regions:
        rects.append(rect.position.x / 1280)
        rects.append(rect.position.y / 720)
        rects.append(rect.size.x / 1280)
        rects.append(rect.size.y / 720)
    lighting.set_ui_regions(rects)

func clear_children(parent: Node) -> void:
    for child: Node in parent.get_children():
        parent.remove_child(child)
        child.queue_free()

func show_main_menu() -> void:
    in_menu = true
    GameState.running = false
    lighting.set_active(false)
    fallback.hide()
    hud.hide()
    close_modal()
    clear_children(menu)
    menu.show()
    var backdrop: Control = BackdropScript.new()
    backdrop.size = Vector2(1280, 720)
    menu.add_child(backdrop)
    label(menu, "AITU  /  INTERACTIVE HORROR", Rect2(64, 53, 700, 30), 14, GOLD)
    label(menu, "01: UNIVERSITY AFTER HOURS", Rect2(64, 148, 650, 24), 12, MUTED)
    label(menu, "THE LAST\nSTUDENT", Rect2(58, 184, 700, 192), 76)
    label(menu, "NIGHT SHIFT  /  ESCAPE", Rect2(64, 391, 600, 32), 21, GOLD)
    label(menu, "One locked campus. Three pieces of ID.\nAnd something that never graduated.", Rect2(64, 442, 580, 65), 19, MUTED)
    button(menu, "START THE NIGHT   >", Rect2(64, 538, 310, 58), start_game, true).grab_focus()
    button(menu, "CONTROLS", Rect2(390, 538, 158, 58), show_controls)
    button(menu, "CREDITS", Rect2(565, 538, 140, 58), show_credits)
    button(menu, "QUIT", Rect2(721, 538, 120, 58), quit_game)
    label(menu, "STUDENT ID   /   REASSEMBLY REQUIRED", Rect2(787, 442, 440, 32), 13, GOLD)
    label(menu, "C++ / OPENGL 3.3 + GODOT 4.7.2", Rect2(64, 666, 800, 24), 12, MUTED)
    label(menu, "~5 MINUTES  /  HEADPHONES RECOMMENDED", Rect2(828, 666, 400, 24), 12, MUTED)

func start_game() -> void:
    GameState.reset()
    DialogueManager.seen.clear()
    first_sighting = false
    threat = 0
    player.position = Vector2(1200, 320)
    player.flashlight = true
    player.is_hiding = false
    camera.position = player.position
    camera.zoom = Vector2(1.65, 1.65)
    for object: Node2D in objects:
        if str(object.definition.id) == "exit": object.definition.tile = 12
    setup_shade()
    shade.position = shade_ai.get_position()
    in_menu = false
    menu.hide()
    hud.show()
    on_spoken("A whole campus, and somehow I still picked the worst place to nap.")
    show_note("02:14 AM / THE LAST STUDENT", str(DialogueManager.lines.intro), "WAKE UP")

func new_modal(kind: String, title: String) -> Panel:
    close_modal()
    modal_kind = kind
    modal = Control.new()
    ui.add_child(modal)
    var dim := ColorRect.new()
    dim.color = Color(0.015, 0.025, 0.04, 0.82)
    dim.size = Vector2(1280, 720)
    modal.add_child(dim)
    var card := panel(modal, Rect2(224, 106, 832, 508), INK, GOLD)
    label(card, "AITU  /  NIGHT SHIFT", Rect2(30, 18, 770, 22), 12, GOLD)
    label(card, title, Rect2(30, 52, 770, 49), 29)
    return card

func close_modal() -> void:
    modal_kind = ""
    if is_instance_valid(modal):
        ui.remove_child(modal)
        modal.queue_free()
    modal = null

func modal_body(card: Panel, text: String, font_size: int) -> RichTextLabel:
    var body := RichTextLabel.new()
    body.position = Vector2(30, 115)
    body.size = Vector2(770, 290)
    body.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
    body.scroll_active = true
    body.add_theme_font_size_override("normal_font_size", font_size)
    body.add_theme_color_override("default_color", TEXT)
    body.text = text
    card.add_child(body)
    return body

func show_note(title: String, text: String, close_text: String = "CLOSE / E") -> void:
    var card := new_modal("note", title)
    modal_body(card, text, 20)
    button(card, close_text, Rect2(30, 420, 260, 56), close_modal, true).grab_focus()

func show_controls() -> void:
    show_note("CONTROLS", "WASD / ARROWS     Move\nMOUSE                       Aim the flashlight\nF                                  Toggle flashlight (light attracts the Shade)\nSHIFT                         Sprint: faster, but the Shade hears you\nE                                  Search, read, hide / leave a hiding spot\nTAB / I                       Inventory: select materials, then Combine\nESC                             Pause / close a panel\nF3                                Native matrices and renderer stats\nF4                                Switch raw GL / C++ texture fallback")

func show_credits() -> void:
    show_note("BUILT FOR THE NIGHT SHIFT", "THE LAST STUDENT: AITU Night Shift - Escape\nComputer Graphics Fundamentals / Astana IT University\n\nOriginal procedural pixel art, PCM audio, writing and game code.\nC++17 renderer: custom GLSL, GLM, GLFW, GLEW, stb_image.\nGodot 4.7.2 + godot-cpp / native visibility and Shade AI.\n\nThird-party notices and original-asset license: CREDITS.md")

func show_pause() -> void:
    var card := new_modal("pause", "THE NIGHT CAN WAIT")
    label(card, "Your progress stays here. Take a breath.", Rect2(30, 115, 770, 50), 20, MUTED)
    button(card, "RESUME", Rect2(30, 190, 360, 56), close_modal, true).grab_focus()
    button(card, "CONTROLS", Rect2(414, 190, 360, 56), show_controls)
    button(card, "MAIN MENU", Rect2(30, 272, 360, 56), show_main_menu)
    button(card, "QUIT", Rect2(414, 272, 360, 56), quit_game)

func show_inventory() -> void:
    var card := new_modal("inventory", "POCKETS / ID REASSEMBLY")
    label(card, "Select the THREE ID PIECES and GLUE. Keys stay in your pocket.", Rect2(30, 107, 770, 26), 16, MUTED)
    var index: int = 0
    for id: String in Inventory.items:
        var item: Dictionary = Inventory.definitions[id]
        var x: int = 30 + (index % 4) * 193
        var y: int = 150 + (index / 4) * 107
        var selected: bool = Inventory.selected.has(id)
        var tile: int = int(item.tile)
        var icon := AtlasTexture.new()
        icon.atlas = world.atlas
        icon.region = Rect2(Vector2(tile % 8, tile >> 3) * 32, Vector2(32, 32))
        var item_button := button(card, ("[X] " if selected else "") + str(item.name), Rect2(x, y, 179, 94), func() -> void: Inventory.toggle_selection(id), selected)
        item_button.add_theme_font_size_override("font_size", 12)
        item_button.icon = icon
        item_button.expand_icon = true
        item_button.add_theme_constant_override("icon_max_width", 32)
        item_button.tooltip_text = str(item.description)
        index += 1
    if Inventory.items.is_empty():
        label(card, "Empty pockets. The most realistic part of university.", Rect2(30, 184, 770, 50), 20)
    var combine_button := button(card, "COMBINE SELECTED", Rect2(30, 414, 380, 58), combine_items, true)
    combine_button.disabled = not Inventory.can_combine()
    button(card, "CLOSE / TAB", Rect2(432, 414, 340, 58), close_modal)

func on_inventory_changed() -> void:
    if modal_kind == "inventory" and not inventory_refresh_pending:
        inventory_refresh_pending = true
        refresh_inventory.call_deferred()

func refresh_inventory() -> void:
    inventory_refresh_pending = false
    if modal_kind == "inventory": show_inventory()

func combine_items() -> void:
    if GameState.combine():
        close_modal()
        AudioManager.play_sound("pickup", -6, 0.7)
        DialogueManager.say("combine")
        effect_timer = 0.55
        flash_color = GOLD
        var assembly := Sprite2D.new()
        var icon := AtlasTexture.new()
        icon.atlas = world.atlas
        icon.region = Rect2(192, 64, 32, 32)
        assembly.texture = icon
        assembly.position = Vector2(640, 360)
        assembly.scale = Vector2(2, 2)
        ui.add_child(assembly)
        var effect := create_tween()
        effect.tween_property(assembly, "scale", Vector2(5, 5), 0.35).set_trans(Tween.TRANS_BACK)
        effect.tween_property(assembly, "modulate:a", 0.0, 0.4)
        effect.tween_callback(assembly.queue_free)
        var tween := create_tween()
        tween.tween_property(camera, "zoom", Vector2(1.9, 1.9), 0.2)
        tween.tween_property(camera, "zoom", Vector2(1.65, 1.65), 0.5)

func show_code() -> void:
    var card := new_modal("code", "COWORKING LOCKER / FOUR DIGITS")
    label(card, "A note on the lock: \"West classroom. Numbers in order.\"", Rect2(30, 122, 770, 45), 20, MUTED)
    var input := LineEdit.new()
    input.position = Vector2(30, 198)
    input.size = Vector2(350, 78)
    input.max_length = 4
    input.placeholder_text = "0000"
    input.add_theme_font_size_override("font_size", 38)
    card.add_child(input)
    var feedback := label(card, "", Rect2(30, 300, 770, 60), 17, GOLD)
    feedback.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
    var submit := func() -> void:
        var result: String = GameState.interact("locker", input.text)
        if result == "piece_2":
            close_modal()
            AudioManager.play_sound("pickup")
            DialogueManager.say("correct_code")
        else:
            feedback.text = str(DialogueManager.lines.wrong_code)
            AudioManager.play_sound("lock")
    input.text_submitted.connect(func(_text: String) -> void: submit.call())
    button(card, "UNLOCK", Rect2(30, 414, 350, 58), submit, true)
    button(card, "BACK", Rect2(414, 414, 360, 58), close_modal)
    input.grab_focus()

func interact() -> void:
    if player.is_hiding:
        player.is_hiding = false
        DialogueManager.say("unhide")
        return
    if not is_instance_valid(nearby): return
    var definition: Dictionary = nearby.definition
    var action: String = str(definition.action)
    if action == "hide":
        player.is_hiding = true
        player.position = nearby.position
        DialogueManager.say("hide")
        return
    if action == "note":
        GameState.read_notes[str(definition.id)] = true
        show_note(str(definition.name).to_upper(), str(definition.note))
        return
    var result: String = GameState.interact(str(definition.id))
    if result == "code": show_code(); return
    DialogueManager.say(result)
    AudioManager.play_sound("pickup" if Inventory.definitions.has(result) else "lock")

func on_spoken(text: String) -> void:
    dialogue_label.text = text

func on_caught() -> void:
    player.position = GameState.checkpoint
    player.is_hiding = false
    camera.position = player.position
    effect_timer = 1.5
    flash_color = Color("a32b35")
    DialogueManager.say("caught")
    AudioManager.play_sound("sting", -5)

func on_escaped() -> void:
    close_modal()
    lighting.set_active(false)
    fallback.hide()
    AudioManager.play_sound("escape", -5)
    for object: Node2D in objects:
        if str(object.definition.id) == "exit": object.definition.tile = 48
    var card := new_modal("ending", "06:03 AM / YOU MADE IT OUT")
    var dim: ColorRect = modal.get_child(0)
    dim.color = Color(0.15,0.19,0.18,0.76)
    modal_body(card, str(DialogueManager.lines.escape) + "\n\nThe deadline is still there. But so is tomorrow.\n\nTIME  %s       CAUGHT  %d\n\nThanks for playing." % [format_time(GameState.elapsed), GameState.catches], 23)
    button(card, "PLAY AGAIN", Rect2(30, 420, 360, 56), start_game, true).grab_focus()
    button(card, "MAIN MENU", Rect2(414, 420, 360, 56), show_main_menu)

func format_time(seconds: float) -> String:
    return "%02d:%02d" % [int(seconds) / 60, int(seconds) % 60]

func _input(event: InputEvent) -> void:
    if not event is InputEventKey or not event.pressed or event.echo: return
    var key: int = event.physical_keycode
    if key == KEY_F3:
        debug_enabled = not debug_enabled
        debug_panel.visible = debug_enabled
        update_ui_regions()
    elif key == KEY_F4:
        raw_enabled = not raw_enabled
        lighting.set_raw(raw_enabled)
    elif key == KEY_ESCAPE:
        if modal_kind == "ending": show_main_menu()
        elif not modal_kind.is_empty(): close_modal()
        elif not in_menu: show_pause()
    elif key in [KEY_TAB, KEY_I] and not in_menu:
        if modal_kind == "inventory": close_modal()
        elif modal_kind.is_empty(): show_inventory()
    elif key == KEY_E and not in_menu:
        if modal_kind == "note": close_modal()
        elif modal_kind.is_empty(): interact()
    elif key == KEY_F and not in_menu and modal_kind.is_empty():
        player.flashlight = not player.flashlight
        AudioManager.play_sound("click")
        DialogueManager.say("flashlight", true)
    else:
        return
    get_viewport().set_input_as_handled()

func _process(delta: float) -> void:
    if not is_instance_valid(player): return
    clock += delta
    smoothed_fps = lerpf(smoothed_fps, 1.0 / maxf(delta, 0.0001), 0.05)
    var playing: bool = not in_menu and modal_kind.is_empty() and not GameState.won
    var in_world: bool = not in_menu and not GameState.won
    GameState.advance_time(delta if playing else 0)
    player.advance(delta, playing)
    if playing:
        GameState.enter_block(1 if player.position.x < 850 else 3 if player.position.x > 1660 else 2)
        var noise: float = 1.0 if player.sprinting else 0.15 if player.moving else 0.0
        var caught: bool = shade_ai.tick(delta, player.position, player.flashlight and not player.is_hiding, player.is_hiding, noise)
        shade.position = shade_ai.get_position()
        if caught: GameState.caught()
        threat = clampf(1.0 - player.position.distance_to(shade.position) / 370.0, 0, 1)
        if player.is_hiding: threat *= 0.35
        shade.chasing = shade_ai.get_state() == "CHASE"
        if threat > 0.56 and not first_sighting:
            first_sighting = true
            DialogueManager.say("shade")
            AudioManager.play_sound("sting", -10)
    var target_camera: Vector2 = player.position + Vector2.from_angle(player.angle) * 45
    camera.position = camera.position.lerp(target_camera, minf(delta * 6, 1))
    effect_timer = maxf(0, effect_timer - delta)
    camera.offset = Vector2(sin(clock * 93), cos(clock * 79)) * effect_timer * 5
    capture_flash.color = Color(flash_color, minf(effect_timer * 0.18, 0.25))
    nearby = null
    var nearest: float = 70
    for object: Node2D in objects:
        object.near = false
        var distance: float = object.position.distance_to(player.position)
        if distance < nearest:
            nearest = distance
            nearby = object
    if is_instance_valid(nearby): nearby.near = true
    prompt_label.text = "[E] Leave hiding spot / you are safe while hidden" if player.is_hiding else "[E] " + str(nearby.definition.name) if is_instance_valid(nearby) else "WASD move    F flashlight    SHIFT sprint (loud)    TAB inventory    ESC pause"
    objective_label.text = GameState.objective()
    status_label.text = "BLOCK 0%d      ID  %d / 3      %s\nTORCH %s     %s" % [GameState.block, Inventory.piece_count(), format_time(GameState.elapsed), "ON" if player.flashlight else "OFF", "HIDDEN" if player.is_hiding else "SPRINT / LOUD" if player.sprinting else "WALK / QUIET"]
    var size: Vector2 = get_viewport_rect().size
    lighting.set_frame(player.position, player.angle, camera.position + camera.offset, camera.zoom.x, player.flashlight and not player.is_hiding, maxf(threat, effect_timer * 0.5), world.lamps, clock, size)
    lighting.set_active(in_world and modal_kind.is_empty())
    var native_stats: Dictionary = lighting.get_stats()
    if bool(native_stats.failed): raw_enabled = false
    fallback.visible = in_world and not raw_enabled
    if fallback.visible:
        mask_timer -= delta
        if mask_timer <= 0:
            fallback_material.set_shader_parameter("native_mask", lighting.get_mask())
            mask_timer = 0.08
        fallback_material.set_shader_parameter("threat", maxf(threat, effect_timer * 0.5))
        fallback_material.set_shader_parameter("clock", clock)
    AudioManager.atmosphere(delta, threat, playing)
    if debug_enabled:
        var matrices: Dictionary = stats.matrices(player.position, player.angle, camera.position + camera.offset, camera.zoom.x, size)
        var text: String = "NATIVE / %s    FPS %d    DRAWS %d    GL ERROR %d\nXY %.1f %.1f    SHADE %s    WALL SEGMENTS %d\n" % [native_stats.path, roundi(smoothed_fps), native_stats.draw_calls, native_stats.gl_error, player.position.x, player.position.y, shade_ai.get_state(), world.edges.size() / 2]
        for name: String in ["model", "view", "projection"]:
            text += name.to_upper() + "\n"
            var m: PackedFloat32Array = matrices[name]
            for row: int in range(4): text += "  %9.3f  %9.3f  %9.3f  %9.3f\n" % [m[row * 4], m[row * 4 + 1], m[row * 4 + 2], m[row * 4 + 3]]
        debug_label.text = text + "F3 hide / F4 rendering path / native frames %d" % native_stats.frames

func capture_run() -> void:
    # Reproducible visible gate evidence; not used by a normal play session.
    for index: int in range(3): await get_tree().process_frame
    await RenderingServer.frame_post_draw
    get_viewport().get_texture().get_image().save_png("res://../artifacts/menu.png")
    start_game()
    close_modal()
    player.position = Vector2(1200, 430)
    camera.position = player.position
    for index: int in range(90): await get_tree().process_frame
    lighting.request_capture("res://../artifacts/gameplay_raw.png")
    for index: int in range(5): await get_tree().process_frame
    debug_enabled = true
    debug_panel.show()
    update_ui_regions()
    for index: int in range(10): await get_tree().process_frame
    lighting.request_capture("res://../artifacts/debug.png")
    for index: int in range(5): await get_tree().process_frame
    debug_enabled = false
    debug_panel.hide()
    update_ui_regions()
    raw_enabled = false
    lighting.set_raw(false)
    for index: int in range(20): await get_tree().process_frame
    await RenderingServer.frame_post_draw
    get_viewport().get_texture().get_image().save_png("res://../artifacts/gameplay_fallback.png")
    for id: String in ["west_key", "east_key", "piece_1", "piece_2", "piece_3", "glue"]: Inventory.add_item(id)
    show_inventory()
    for index: int in range(5): await get_tree().process_frame
    await RenderingServer.frame_post_draw
    get_viewport().get_texture().get_image().save_png("res://../artifacts/inventory.png")
    close_modal()
    for id: String in ["piece_1", "piece_2", "piece_3", "glue"]: Inventory.toggle_selection(id)
    combine_items()
    GameState.elapsed = 297
    GameState.interact("exit")
    for index: int in range(5): await get_tree().process_frame
    await RenderingServer.frame_post_draw
    get_viewport().get_texture().get_image().save_png("res://../artifacts/ending.png")
    print("RENDER_GATE ", lighting.get_stats())
    AudioManager.muted = true
    AudioManager.stop_all()
    await get_tree().create_timer(0.3).timeout
    get_tree().quit()

func _notification(what: int) -> void:
    if what == NOTIFICATION_WM_CLOSE_REQUEST: quit_game()

func quit_game() -> void:
    if quitting: return
    quitting = true
    lighting.set_active(false)
    AudioManager.muted = true
    AudioManager.stop_all()
    await get_tree().create_timer(0.2).timeout
    get_tree().quit()

func benchmark_run() -> void:
    AudioManager.muted = true
    start_game()
    close_modal()
    for index: int in range(120): await get_tree().process_frame
    var samples: Array[float] = []
    var before: int = Time.get_ticks_usec()
    for index: int in range(600):
        await get_tree().process_frame
        var now: int = Time.get_ticks_usec()
        samples.append(float(now - before) / 1000)
        before = now
    samples.sort()
    var total: float = 0
    for value: float in samples: total += value
    var result: Dictionary = {"frames": samples.size(), "mean_ms": total / samples.size(), "p95_ms": samples[570], "mean_fps": 600000.0 / total, "renderer": lighting.get_stats(), "gpu": RenderingServer.get_video_adapter_name(), "size": get_viewport_rect().size}
    print("BENCHMARK ", result)
    var output := FileAccess.open("res://../artifacts/benchmark.json", FileAccess.WRITE)
    output.store_string(JSON.stringify(result, "  "))
    output.close()
    lighting.set_active(false)
    get_tree().quit()
