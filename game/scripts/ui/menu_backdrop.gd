extends Control
## Procedural campus silhouette and a torn-ID motif, independent of external art.
func _draw() -> void:
    draw_rect(Rect2(0, 0, 1280, 720), Color("0b1420"))
    for row: int in range(14):
        for col: int in range(24):
            var x: float = col * 55 + 400
            var y: float = row * 38 + 240
            var lit: bool = posmod(col * 17 + row * 31, 19) < 3
            draw_rect(Rect2(x, y, 46, 30), Color("273b42") if lit else Color("101f2b"))
            draw_line(Vector2(x + 23, y), Vector2(x + 23, y + 30), Color("0c1721"), 2)
    draw_rect(Rect2(0, 0, 670, 720), Color(0.035, 0.065, 0.10, 0.93))
    draw_rect(Rect2(0, 0, 1280, 720), Color(0.02, 0.04, 0.07, 0.23))
    for y: int in range(0, 720, 4):
        draw_line(Vector2(0, y), Vector2(1280, y), Color(0, 0, 0, 0.08), 1)
    draw_line(Vector2(64, 100), Vector2(1216, 100), Color("344951"), 1)
    draw_line(Vector2(64, 651), Vector2(1216, 651), Color("344951"), 1)
    # Three disconnected card fragments.
    draw_set_transform(Vector2(958, 325), -0.12, Vector2.ONE)
    draw_polygon(PackedVector2Array([Vector2(-139, -68), Vector2(-20, -68), Vector2(-9, 7), Vector2(-27, 70), Vector2(-139, 70)]), PackedColorArray([Color("8da69a")]))
    draw_polygon(PackedVector2Array([Vector2(-2, -73), Vector2(132, -73), Vector2(132, -2), Vector2(22, 0)]), PackedColorArray([Color("a2b2a0")]))
    draw_polygon(PackedVector2Array([Vector2(18, 20), Vector2(145, 15), Vector2(145, 85), Vector2(3, 85)]), PackedColorArray([Color("687f80")]))
    draw_rect(Rect2(-121, -37, 66, 76), Color("344d59"))
    draw_circle(Vector2(-88, -10), 15, Color("90a59c"))
    draw_rect(Rect2(-108, 9, 40, 28), Color("90a59c"))
    draw_line(Vector2(15, -48), Vector2(113, -48), Color("344e54"), 7)
    draw_line(Vector2(20, -25), Vector2(92, -25), Color("526b69"), 3)
    for i: int in range(18):
        draw_line(Vector2(21 + i * 6, 38), Vector2(21 + i * 6, 62), Color("314953"), 2 if i % 3 else 3)
    draw_set_transform(Vector2.ZERO)
