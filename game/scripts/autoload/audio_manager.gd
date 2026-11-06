extends Node
## Pooled voices; hum loops seamlessly, heartbeat cadence follows threat.
var voices: Array[AudioStreamPlayer] = []
var sounds: Dictionary = {}
var hum: AudioStreamPlayer
var beat_timer: float = 0.0
var muted: bool = false
var shutting_down: bool = false

func _ready() -> void:
    for name: String in ["hum", "step", "heartbeat", "click", "pickup", "lock", "sting", "escape"]:
        sounds[name] = load("res://assets/audio/" + name + ".wav")
    for index: int in range(8):
        var voice := AudioStreamPlayer.new()
        voice.playback_type = AudioServer.PLAYBACK_TYPE_STREAM
        add_child(voice)
        voices.append(voice)
    hum = AudioStreamPlayer.new()
    hum.playback_type = AudioServer.PLAYBACK_TYPE_STREAM
    hum.stream = sounds["hum"]
    hum.volume_db = -24
    add_child(hum)
    hum.finished.connect(func() -> void:
        if not shutting_down: hum.play())

func stop_all() -> void:
    shutting_down = true
    if is_instance_valid(hum):
        hum.stop()
        hum.stream = null
    for voice: AudioStreamPlayer in voices:
        voice.stop()
        voice.stream = null

func _exit_tree() -> void:
    stop_all()
    sounds.clear()

func play_sound(name: String, volume: float = -10.0, pitch: float = 1.0) -> void:
    if muted or not sounds.has(name):
        return
    for voice: AudioStreamPlayer in voices:
        if not voice.playing:
            voice.stream = sounds[name]
            voice.volume_db = volume
            voice.pitch_scale = pitch
            voice.play()
            return

func atmosphere(delta: float, threat: float, enabled: bool) -> void:
    if enabled and not muted:
        if not hum.playing:
            hum.play()
        beat_timer -= delta
        if threat > 0.14 and beat_timer <= 0.0:
            play_sound("heartbeat", lerpf(-30.0, -8.0, threat))
            beat_timer = lerpf(1.5, 0.50, threat)
    else:
        hum.stop()
