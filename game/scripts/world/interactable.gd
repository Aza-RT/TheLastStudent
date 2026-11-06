extends Node2D
## A data-driven quest object; nearby highlighting is independent of item logic.
var definition: Dictionary = {}
var near: bool = false
var pulse: float = 0.0
var atlas: Texture2D = preload("res://assets/art/atlas.png")

func _process(delta: float) -> void:
    pulse += delta
    queue_redraw()

func _draw() -> void:
    if definition.is_empty(): return
    var done: bool = GameState.searched.has(str(definition.id))
    var tile: int = int(definition.tile)
    var region := Rect2(Vector2(tile % 8, tile >> 3) * 32, Vector2(32, 32))
    draw_texture_rect_region(atlas, Rect2(-20, -20, 40, 40), region, Color("77918a") if done else Color.WHITE)
    if near:
        draw_arc(Vector2.ZERO, 26, 0, TAU, 32, Color("d7c187"), 1.5, false)
    elif not done and str(definition.action) != "hide":
        var alpha: float = 0.4 + 0.25 * sin(pulse * 2)
        draw_circle(Vector2(16, -20), 2.5, Color(0.83, 0.76, 0.52, alpha))
