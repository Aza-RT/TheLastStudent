extends Node2D
class_name CampusWorld
## Static tile blockout and wall edges come from one editable map data file.
const TILE: int = 32
var data: Dictionary
var floor_cells: Dictionary = {}
var wall_cells: Dictionary = {}
var edges := PackedVector2Array()
var lamps := PackedVector2Array()
var atlas: Texture2D = preload("res://assets/art/atlas.png")
var room_font: Font = ThemeDB.fallback_font

func _ready() -> void:
    data = JSON.parse_string(FileAccess.get_file_as_string("res://data/map_layout.json"))
    paint_rect(data.corridor)
    for room: Dictionary in data.rooms:
        paint_rect(room.rect)
    for door: Array in data.doors:
        paint_rect(door)
    extract_walls()
    for p: Array in data.lamps:
        lamps.append(Vector2(p[0], p[1]))
    queue_redraw()

func paint_rect(rect: Array) -> void:
    for y: int in range(rect[1], rect[1] + rect[3]):
        for x: int in range(rect[0], rect[0] + rect[2]):
            floor_cells[Vector2i(x, y)] = true

func extract_walls() -> void:
    # Merge adjacent collinear edges to keep ray counts and GL fan sizes bounded.
    var horizontal: Dictionary = {}
    var vertical: Dictionary = {}
    for cell: Vector2i in floor_cells:
        for direction: Vector2i in [Vector2i.UP, Vector2i.DOWN, Vector2i.LEFT, Vector2i.RIGHT]:
            if floor_cells.has(cell + direction):
                continue
            wall_cells[cell + direction] = true
            if direction.y != 0:
                var y: int = cell.y + int(direction.y > 0)
                if not horizontal.has(y): horizontal[y] = []
                horizontal[y].append(cell.x)
            else:
                var x: int = cell.x + int(direction.x > 0)
                if not vertical.has(x): vertical[x] = []
                vertical[x].append(cell.y)
    merge_edges(horizontal, true)
    merge_edges(vertical, false)

func merge_edges(groups: Dictionary, horizontal: bool) -> void:
    for axis: int in groups:
        var values: Array = groups[axis]
        values.sort()
        var first: int = values[0]
        var last: int = first
        for value: int in values.slice(1):
            if value != last + 1:
                append_edge(axis, first, last + 1, horizontal)
                first = value
            last = value
        append_edge(axis, first, last + 1, horizontal)

func append_edge(axis: int, first: int, last: int, horizontal: bool) -> void:
    if horizontal:
        edges.append(Vector2(first * TILE, axis * TILE))
        edges.append(Vector2(last * TILE, axis * TILE))
    else:
        edges.append(Vector2(axis * TILE, first * TILE))
        edges.append(Vector2(axis * TILE, last * TILE))

func can_stand(position_world: Vector2) -> bool:
    for offset: Vector2 in [Vector2(-8, -8), Vector2(8, -8), Vector2(-8, 8), Vector2(8, 8)]:
        if not floor_cells.has(Vector2i((position_world + offset).floor() / TILE)):
            return false
    return true

func draw_tile(tile: int, position_world: Vector2, size: Vector2 = Vector2(32, 32), tint: Color = Color.WHITE) -> void:
    var region := Rect2(Vector2((tile % 8) * 32, (tile >> 3) * 32), Vector2(32, 32))
    draw_texture_rect_region(atlas, Rect2(position_world - size * 0.5, size), region, tint)

func _draw() -> void:
    if data.is_empty(): return
    for cell: Vector2i in floor_cells:
        draw_tile(posmod(cell.x * 13 + cell.y * 7, 3), Vector2(cell * TILE) + Vector2(16, 16))
    for cell: Vector2i in wall_cells:
        draw_tile(3, Vector2(cell * TILE) + Vector2(16, 16))
    for room: Dictionary in data.rooms:
        var rect: Array = room.rect
        var p := Vector2(rect[0] * TILE + 12, rect[1] * TILE + 25)
        draw_string(room_font, p, str(room.name), HORIZONTAL_ALIGNMENT_LEFT, -1, 14, Color("a5c0b8"))
        draw_line(p + Vector2(0, 10), p + Vector2(120, 10), Color("6b8b84"), 1)
    for prop: Array in data.decor:
        draw_tile(prop[2], Vector2(prop[0] * TILE + 16, prop[1] * TILE + 16))
    for block: int in range(3):
        var p := Vector2(140 + block * 800, 600)
        draw_string(room_font, p, "BLOCK 0%d    /    AITU" % (block + 1), HORIZONTAL_ALIGNMENT_LEFT, -1, 13, Color("84978d"))
    # Exit runway / floor markings.
    draw_line(Vector2(1120, 944), Vector2(1120, 1000), Color("c7ae72"), 2)
    draw_line(Vector2(1312, 944), Vector2(1312, 1000), Color("c7ae72"), 2)
