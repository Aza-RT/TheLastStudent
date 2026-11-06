extends Node2D
class_name StudentPlayer
## World-space movement, collision and four-direction sprite animation.
var world: CampusWorld
var angle: float = 0.0
var flashlight: bool = true
var is_hiding: bool = false
var moving: bool = false
var sprinting: bool = false
var step_timer: float = 0.0
var walk_time: float = 0.0
var atlas: Texture2D = preload("res://assets/art/atlas.png")

func advance(delta: float, enabled: bool) -> void:
    angle = (get_global_mouse_position() - global_position).angle()
    var input_vector := Vector2.ZERO
    if enabled and not is_hiding:
        input_vector.x = float(Input.is_physical_key_pressed(KEY_D) or Input.is_physical_key_pressed(KEY_RIGHT)) - float(Input.is_physical_key_pressed(KEY_A) or Input.is_physical_key_pressed(KEY_LEFT))
        input_vector.y = float(Input.is_physical_key_pressed(KEY_S) or Input.is_physical_key_pressed(KEY_DOWN)) - float(Input.is_physical_key_pressed(KEY_W) or Input.is_physical_key_pressed(KEY_UP))
    moving = input_vector.length_squared() > 0.0
    sprinting = moving and Input.is_physical_key_pressed(KEY_SHIFT)
    var speed: float = 210.0 if sprinting else 125.0
    var velocity := input_vector.normalized() * speed * minf(delta, 0.05)
    if world.can_stand(position + Vector2(velocity.x, 0)): position.x += velocity.x
    if world.can_stand(position + Vector2(0, velocity.y)): position.y += velocity.y
    if moving:
        walk_time += delta * (12.0 if sprinting else 8.0)
        step_timer -= delta
        if step_timer <= 0:
            AudioManager.play_sound("step", -14.0 if sprinting else -26.0, randf_range(0.9, 1.1))
            step_timer = 0.25 if sprinting else 0.42
    queue_redraw()

func _draw() -> void:
    var direction: int = 1
    if absf(cos(angle)) > absf(sin(angle)):
        direction = 2 if cos(angle) > 0 else 3
    else:
        direction = 1 if sin(angle) > 0 else 0
    var frame: int = int(walk_time) % 4 if moving else 0
    var tile: int = 32 + direction * 4 + frame
    var region := Rect2(Vector2(tile % 8, tile >> 3) * 32, Vector2(32, 32))
    draw_circle(Vector2(0, 7), 12, Color(0, 0, 0, 0.28))
    draw_texture_rect_region(atlas, Rect2(-16, -20, 32, 32), region, Color(0.6, 0.8, 0.8, 0.4) if is_hiding else Color.WHITE)
    if flashlight and not is_hiding:
        var tip := Vector2.from_angle(angle) * 14
        draw_line(tip, tip + Vector2.from_angle(angle) * 5, Color("d7e8be"), 3)
