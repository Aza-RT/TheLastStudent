extends SceneTree
## Headless integration test: real scene, reachable quest objects, real inventory/quest API.
var checks: int = 0
var failed: bool = false

func check(condition: bool, message: String) -> void:
    checks += 1
    if not condition:
        failed = true
        push_error("SMOKE FAIL: " + message)

func _initialize() -> void:
    run.call_deferred()

func reachable_cells(world: CampusWorld, origin: Vector2i) -> Dictionary:
    var found: Dictionary = {origin: true}
    var queue: Array[Vector2i] = [origin]
    var cursor: int = 0
    while cursor < queue.size():
        var current: Vector2i = queue[cursor]
        cursor += 1
        for direction: Vector2i in [Vector2i.UP, Vector2i.DOWN, Vector2i.LEFT, Vector2i.RIGHT]:
            var next: Vector2i = current + direction
            if world.floor_cells.has(next) and not found.has(next):
                found[next] = true
                queue.append(next)
    return found

func run() -> void:
    root.get_node("AudioManager").muted = true
    var game = load("res://scenes/main/main.tscn").instantiate()
    var state = root.get_node("GameState")
    var inventory = root.get_node("Inventory")
    root.add_child(game)
    await process_frame
    game.start_game()
    game.close_modal()
    # Exercise the same key handler that runs before GUI focus consumes TAB.
    var key := InputEventKey.new()
    key.pressed = true
    key.physical_keycode = KEY_TAB
    game._input(key)
    check(game.modal_kind == "inventory", "TAB opens inventory")
    game._input(key)
    check(game.modal_kind.is_empty(), "TAB closes focused inventory")
    key.physical_keycode = KEY_ESCAPE
    game._input(key)
    check(game.modal_kind == "pause", "ESC opens pause")
    game._input(key)
    check(game.modal_kind.is_empty(), "ESC resumes")
    var torch_before: bool = game.player.flashlight
    key.physical_keycode = KEY_F
    game._input(key)
    check(game.player.flashlight != torch_before, "F toggles flashlight")
    for object: Node2D in game.objects:
        if str(object.definition.id) == "hide_hub":
            game.nearby = object
            break
    game.interact()
    check(game.player.is_hiding, "E enters safe hiding spot")
    game.interact()
    check(not game.player.is_hiding, "E leaves hiding spot")
    for id: String in ["piece_1", "piece_2", "piece_3", "glue"]: inventory.add_item(id)
    game.show_inventory()
    var card: Panel = game.modal.get_child(1)
    for child: Node in card.get_children():
        if child is Button and (child.text.begins_with("ID card") or child.text == "Glue stick"):
            child.pressed.emit()
    check(inventory.can_combine(), "four actual item buttons select distinct materials")
    game.combine_items()
    await process_frame
    check(inventory.has_item("restored_id") and game.modal_kind.is_empty(), "assembly closes inventory without deferred reopening")
    # Freeze ordinary simulation while inspecting deterministic quest state.
    game.set_process(false)
    var world: CampusWorld = game.world
    check(world.edges.size() > 20, "merged occluder segments extracted")
    var reached := reachable_cells(world, Vector2i(37, 10))
    for object: Node2D in game.objects:
        check(reached.has(Vector2i(object.position / 32)), "reachable object: " + str(object.definition.id))
    check(not world.can_stand(Vector2(20,20)), "solid exterior blocks movement")
    check(world.can_stand(Vector2(1200,320)), "start tile accepts player")
    state.reset()
    check(state.interact("exit") == "exit_locked", "exit rejects missing ID")
    check(state.interact("library_drawer") == "locked_library_drawer", "library key required")
    check(state.interact("fridge") == "locked_fridge", "canteen key required")
    check(state.interact("locker", "0000") == "wrong_code", "wrong code rejects")
    check(state.interact("west_key") == "west_key", "west restroom key")
    check(state.interact("west_key") == "empty", "no duplicate collection")
    check(state.interact("library_drawer") == "piece_1", "block 1 ID piece")
    state.enter_block(1)
    var kept: Array = inventory.items.duplicate()
    state.caught()
    check(state.catches == 1 and inventory.items == kept, "catch retains all inventory")
    check(game.player.position == state.checkpoint, "catch respawns at block checkpoint")
    check(state.interact("locker", "2410") == "piece_2", "block 2 code puzzle")
    state.enter_block(3)
    check(state.interact("east_key") == "east_key", "east classroom key")
    check(state.interact("glue") == "glue", "east classroom glue")
    check(state.interact("fridge") == "piece_3", "block 3 ID piece")
    check(inventory.piece_count() == 3, "exactly three ID pieces")
    check(not state.combine(), "combine requires selected materials")
    for id: String in ["piece_1", "piece_2", "piece_3", "glue"]: inventory.toggle_selection(id)
    check(state.combine(), "selected materials assemble restored ID")
    check(inventory.has_item("restored_id") and not inventory.has_item("glue"), "assembly consumes materials")
    check(state.interact("exit") == "escape", "restored ID unlocks ending")
    check(state.won and game.modal_kind == "ending", "ending screen triggered")
    check(state.catches == 1, "ending preserves catch count")
    game.show_main_menu()
    game.show_controls()
    game.close_modal()
    game.show_credits()
    game.close_modal()
    game.start_game()
    game.close_modal()
    game.show_inventory()
    check(game.modal_kind == "inventory", "inventory screen opens")
    game.close_modal()
    game.show_pause()
    check(game.modal_kind == "pause", "pause menu opens")
    game.close_modal()
    game.show_code()
    check(game.modal_kind == "code", "keypad screen opens")
    game.close_modal()
    check(not state.won and inventory.items.is_empty(), "restart clears previous run")
    for object: Node2D in game.objects:
        if str(object.definition.id) == "exit": check(int(object.definition.tile) == 12, "replay resets locked turnstile artwork")
    # Every authored scene resource must be loadable.
    for scene: String in ["block1", "block2", "block3", "corridors"]:
        var resource: PackedScene = load("res://scenes/world/" + scene + ".tscn")
        check(resource != null and resource.can_instantiate(), "scene loads: " + scene)
    print("PUZZLE_SMOKE ", "FAIL" if failed else "PASS", " / ", checks, " checks / reachable cells ", reached.size())
    root.get_node("AudioManager").stop_all()
    game.queue_free()
    await create_timer(0.15).timeout
    quit(1 if failed else 0)
