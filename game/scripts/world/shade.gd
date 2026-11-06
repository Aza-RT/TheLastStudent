extends Node2D
var atlas: Texture2D = preload("res://assets/art/atlas.png")
var age: float = 0.0
var chasing: bool = false

func _process(delta: float) -> void:
    age += delta
    queue_redraw()

func _draw() -> void:
    var tile: int = 24 + int(age * 5) % 4
    draw_circle(Vector2(0, 7), 18, Color(0.04, 0.05, 0.09, 0.5))
    draw_texture_rect_region(atlas, Rect2(-22, -28 + sin(age * 3) * 3, 44, 44), Rect2(Vector2(tile % 8, tile >> 3) * 32, Vector2(32, 32)), Color("c5aaa6") if chasing else Color("a2bbc4"))
