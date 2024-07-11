extends Control

var current_path = "Untitled"
var main_path = ""

# Called when the node enters the scene tree for the first time.
func _ready():
	# Run lsblk command to get device information
	var cmd = "lsblk"
	var output = []
	var usb_path
	OS.execute(cmd, [], output)  # Execute command and capture output
	
	 #Split output by lines
	var lines = output[0].split("\n")
	
	# Iterate over each line to find the line containing the USB device
	var usb_device_line = null
	for line in lines:
		if line.find("/media/") != -1:
			usb_device_line = line.substr(line.find("/media/"))
			print("line : " + usb_device_line)
			break
	
	if usb_device_line != null:
		# Extract the mount point path
		main_path = usb_device_line + "/"
		print("USB Path:", main_path)
		dir_contents(main_path)
	else:
		print("USB device not found or not mounted")
	update_window_title()
	
	
func update_window_title():
	DisplayServer.window_set_title("USB Read Write - " + current_path.replacen(main_path, "usb"))
	
func dir_contents(path):
	var dir = DirAccess.open(path)
	if dir:
		dir.list_dir_begin()
		var file_name = dir.get_next()
		while file_name != "":
			if dir.current_is_dir():
				print("Found directory: " + file_name)
				var label = Label.new()
				label.text = path.replacen(main_path, "usb") + "/" + file_name
				$ColorRect/MarginContainer/HBoxContainer/ScrollContainer/DirectoryContainer.add_child(label)
				dir_contents(path + "/" + file_name)
			else:
				print("Found file: " + file_name)
				var btn = Button.new()
				btn.pressed.connect(_open_file.bind(path + '/' + file_name))
				btn.text = file_name
				
				$ColorRect/MarginContainer/HBoxContainer/ScrollContainer/DirectoryContainer.add_child(btn)
			file_name = dir.get_next()
		var label2 = Label.new()
		label2.text = "~" + path.replacen(main_path, "usb")
		$ColorRect/MarginContainer/HBoxContainer/ScrollContainer/DirectoryContainer.add_child(label2)
		dir.list_dir_end()  # Close the directory listing
	else:
		print("An error occurred when trying to access the path: " + path)
	
		
func _open_file(path):
	print("Opening file: " + path)
	var file = FileAccess.open(path, FileAccess.READ)
	if file:
		current_path = path
		$ColorRect/MarginContainer/HBoxContainer/TextEdit.text = file.get_as_text()
		file.close()
		update_window_title()
	else:
		print("Failed to open file: " + path)
	

func _on_save_button_pressed():
	var path = current_path
	if path == "Untitled":
		_on_save_as_button_pressed()
	else:
		var file = FileAccess.open(path, FileAccess.WRITE)
		if file:
			file.store_string($ColorRect/MarginContainer/HBoxContainer/TextEdit.text)
			file.close()
		else:
			print("Failed to save file: " + path)

func _on_reload_button_pressed():
	var children = $ColorRect/MarginContainer/HBoxContainer/ScrollContainer/DirectoryContainer.get_children()
	for child in children:
		$ColorRect/MarginContainer/HBoxContainer/ScrollContainer/DirectoryContainer.remove_child(child)
	dir_contents('/media/elviin/6C1A0CCD1A0C95EC')

func _on_new_button_pressed():
	current_path = "Untitled"
	$ColorRect/MarginContainer/HBoxContainer/TextEdit.text = ""
	update_window_title()

func _on_save_as_button_pressed():
	$ColorRect/SaveFileDialog.popup()

func _on_save_file_dialog_file_selected(path):
	current_path = path
	var file = FileAccess.open(path, FileAccess.WRITE)
	if file:
		file.store_string($ColorRect/MarginContainer/HBoxContainer/TextEdit.text)
		file.close()
		update_window_title()
		_on_reload_button_pressed()
	else:
		print("Failed to save file as: " + path)

func _on_delete_button_pressed():
	if current_path != "Untitled":
		var result = DirAccess.remove_absolute(current_path)
		if result == OK:
			$ColorRect/MarginContainer/HBoxContainer/TextEdit.text = ""
			current_path = "Untitled"
			update_window_title()
			_on_reload_button_pressed()
		else:
			print("Failed to delete file: " + current_path)
	else:
		print("No file selected for deletion")
