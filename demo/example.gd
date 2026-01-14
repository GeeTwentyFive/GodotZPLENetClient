extends Node


@onready var zec := ZPLENetClient.new()


func _ready() -> void:
	if (zec.connect_to("::1", 55555, 1) != OK):
		print("Failed to connect")
		get_tree().quit(1)
	zec.disconnected.connect(func():
		print("Disconnected")
		get_tree().quit(0)
	)
	
	if (zec.send("Example data".to_ascii_buffer(), 0) != OK):
		print("Failed to send")
		get_tree().quit(1)

func _process(_delta: float) -> void:
	var received_data: Array = zec.service()
	for data in received_data:
		print(data.slice(1).get_string_from_ascii())
		#print(data[1].get_string_from_ascii())
