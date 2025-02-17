GDPC                                                                                       	   T   res://.godot/exported/133200997/export-0a857133a7287c847b17adacf639a0ac-control.scn         �      =�M=@ ŧO�5:�,    ,   res://.godot/global_script_class_cache.cfg  �)             ��Р�8���8~$}P�    D   res://.godot/imported/icon.svg-218a8f2b3041327d8a5756f3a245f83b.ctexP            ：Qt�E�cO���       res://.godot/uid_cache.bin  �-      C       c�Ŗ�F����E��u       res://Control.gd�      T      ���nr��haj��q��        res://Test 1/control.tscn.remap @)      d       �NInМ���������       res://icon.svg  �)      �      k����X3Y���f       res://icon.svg.import   p(      �       7B<�{T��8�/
p       res://project.binary�-            t�e5}Zrmh$V�T��            RSRC                    PackedScene            ��������                                                  resource_local_to_scene    resource_name 	   _bundled    script       Script    res://Control.gd ��������      local://PackedScene_fgm1p          PackedScene          	         names "   8      Control    layout_mode    anchors_preset    anchor_right    anchor_bottom    grow_horizontal    grow_vertical    script 
   ColorRect    color    MarginContainer %   theme_override_constants/margin_left $   theme_override_constants/margin_top &   theme_override_constants/margin_right '   theme_override_constants/margin_bottom    HBoxContainer    ScrollContainer    custom_minimum_size    follow_focus    horizontal_scroll_mode    DirectoryContainer $   theme_override_constants/separation    VBoxContainer 	   TextEdit    size_flags_horizontal    minimap_draw 
   draw_tabs    draw_spaces    gutters_draw_line_numbers    gutters_zero_pad_line_numbers    code_completion_enabled    auto_brace_completion_enabled )   auto_brace_completion_highlight_matching 	   CodeEdit    ButtonContainer 
   NewButton    text    Button    SaveButton    SaveAsButton    DeleteButton    ReloadButton    SaveFileDialog    title    size    access    root_subfolder    FileDialog    _on_new_button_pressed    pressed    _on_save_button_pressed    _on_save_as_button_pressed    _on_delete_button_pressed    _on_reload_button_pressed #   _on_save_file_dialog_file_selected    file_selected    	   variants                        �?                         33�>33�>33�>  �?   
   
     �C                       
     C          New       Save       Save As       Delete       Reload
       Save File As -   �  �        /media/elviin/6C1A0CCD1A0C95EC       node_count             nodes     �   ��������        ����                                                                ����                                       	                 
   
   ����
                                                                                ����                          ����                  	      
                    ����                                !      ����
                   	      	      	      	      	      	      	       	                 "   ����                          %   #   ����         $                 %   &   ����         $                 %   '   ����         $                 %   (   ����         $                 %   )   ����         $                 /   *   ����   +      ,      -      .                conn_count             conns     *          1   0              	       1   2              
       1   3                     1   4                     1   5                     7   6                    node_paths              editable_instances              version             RSRC     extends Control

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
            GST2   �   �      ����               � �        �  RIFF�  WEBPVP8L�  /������!"2�H�m�m۬�}�p,��5xi�d�M���)3��$�V������3���$G�$2#�Z��v{Z�lێ=W�~� �����d�vF���h���ڋ��F����1��ڶ�i�엵���bVff3/���Vff���Ҿ%���qd���m�J�}����t�"<�,���`B �m���]ILb�����Cp�F�D�=���c*��XA6���$
2#�E.@$���A.T�p )��#L��;Ev9	Б )��D)�f(qA�r�3A�,#ѐA6��npy:<ƨ�Ӱ����dK���|��m�v�N�>��n�e�(�	>����ٍ!x��y�:��9��4�C���#�Ka���9�i]9m��h�{Bb�k@�t��:s����¼@>&�r� ��w�GA����ը>�l�;��:�
�wT���]�i]zݥ~@o��>l�|�2�Ż}�:�S�;5�-�¸ߥW�vi�OA�x��Wwk�f��{�+�h�i�
4�˰^91��z�8�(��yޔ7֛�;0����^en2�2i�s�)3�E�f��Lt�YZ���f-�[u2}��^q����P��r��v��
�Dd��ݷ@��&���F2�%�XZ!�5�.s�:�!�Њ�Ǝ��(��e!m��E$IQ�=VX'�E1oܪì�v��47�Fы�K챂D�Z�#[1-�7�Js��!�W.3׹p���R�R�Ctb������y��lT ��Z�4�729f�Ј)w��T0Ĕ�ix�\�b�9�<%�#Ɩs�Z�O�mjX �qZ0W����E�Y�ڨD!�$G�v����BJ�f|pq8��5�g�o��9�l�?���Q˝+U�	>�7�K��z�t����n�H�+��FbQ9���3g-UCv���-�n�*���E��A�҂
�Dʶ� ��WA�d�j��+�5�Ȓ���"���n�U��^�����$G��WX+\^�"�h.���M�3�e.
����MX�K,�Jfѕ*N�^�o2��:ՙ�#o�e.
��p�"<W22ENd�4B�V4x0=حZ�y����\^�J��dg��_4�oW�d�ĭ:Q��7c�ڡ��
A>��E�q�e-��2�=Ϲkh���*���jh�?4�QK��y@'�����zu;<-��|�����Y٠m|�+ۡII+^���L5j+�QK]����I �y��[�����(}�*>+���$��A3�EPg�K{��_;�v�K@���U��� gO��g��F� ���gW� �#J$��U~��-��u���������N�@���2@1��Vs���Ŷ`����Dd$R�":$ x��@�t���+D�}� \F�|��h��>�B�����B#�*6��  ��:���< ���=�P!���G@0��a��N�D�'hX�׀ "5#�l"j߸��n������w@ K�@A3�c s`\���J2�@#�_ 8�����I1�&��EN � 3T�����MEp9N�@�B���?ϓb�C��� � ��+�����N-s�M�  ��k���yA 7 �%@��&��c��� �4�{� � �����"(�ԗ�� �t�!"��TJN�2�O~� fB�R3?�������`��@�f!zD��%|��Z��ʈX��Ǐ�^�b��#5� }ى`�u�S6�F�"'U�JB/!5�>ԫ�������/��;	��O�!z����@�/�'�F�D"#��h�a �׆\-������ Xf  @ �q�`��鎊��M��T�� ���0���}�x^�����.�s�l�>�.�O��J�d/F�ě|+^�3�BS����>2S����L�2ޣm�=�Έ���[��6>���TъÞ.<m�3^iжC���D5�抺�����wO"F�Qv�ږ�Po͕ʾ��"��B��כS�p�
��E1e�������*c�������v���%'ž��&=�Y�ް>1�/E������}�_��#��|������ФT7׉����u������>����0����緗?47�j�b^�7�ě�5�7�����|t�H�Ե�1#�~��>�̮�|/y�,ol�|o.��QJ rmϘO���:��n�ϯ�1�Z��ը�u9�A������Yg��a�\���x���l���(����L��a��q��%`�O6~1�9���d�O{�Vd��	��r\�՜Yd$�,�P'�~�|Z!�v{�N�`���T����3?DwD��X3l �����*����7l�h����	;�ߚ�;h���i�0�6	>��-�/�&}% %��8���=+��N�1�Ye��宠p�kb_����$P�i�5�]��:��Wb�����������ě|��[3l����`��# -���KQ�W�O��eǛ�"�7�Ƭ�љ�WZ�:|���є9�Y5�m7�����o������F^ߋ������������������Р��Ze�>�������������?H^����&=����~�?ڭ�>���Np�3��~���J�5jk�5!ˀ�"�aM��Z%�-,�QU⃳����m����:�#��������<�o�����ۇ���ˇ/�u�S9��������ٲG}��?~<�]��?>��u��9��_7=}�����~����jN���2�%>�K�C�T���"������Ģ~$�Cc�J�I�s�? wڻU���ə��KJ7����+U%��$x�6
�$0�T����E45������G���U7�3��Z��󴘶�L�������^	dW{q����d�lQ-��u.�:{�������Q��_'�X*�e�:�7��.1�#���(� �k����E�Q��=�	�:e[����u��	�*�PF%*"+B��QKc˪�:Y��ـĘ��ʴ�b�1�������\w����n���l镲��l��i#����!WĶ��L}rեm|�{�\�<mۇ�B�HQ���m�����x�a�j9.�cRD�@��fi9O�.e�@�+�4�<�������v4�[���#bD�j��W����֢4�[>.�c�1-�R�����N�v��[�O�>��v�e�66$����P
�HQ��9���r�	5FO� �<���1f����kH���e�;����ˆB�1C���j@��qdK|
����4ŧ�f�Q��+�     [remap]

importer="texture"
type="CompressedTexture2D"
uid="uid://boiu6hdf0pby6"
path="res://.godot/imported/icon.svg-218a8f2b3041327d8a5756f3a245f83b.ctex"
metadata={
"vram_texture": false
}
                [remap]

path="res://.godot/exported/133200997/export-0a857133a7287c847b17adacf639a0ac-control.scn"
            list=Array[Dictionary]([])
     <svg height="128" width="128" xmlns="http://www.w3.org/2000/svg"><rect x="2" y="2" width="124" height="124" rx="14" fill="#363d52" stroke="#212532" stroke-width="4"/><g transform="scale(.101) translate(122 122)"><g fill="#fff"><path d="M105 673v33q407 354 814 0v-33z"/><path d="m105 673 152 14q12 1 15 14l4 67 132 10 8-61q2-11 15-15h162q13 4 15 15l8 61 132-10 4-67q3-13 15-14l152-14V427q30-39 56-81-35-59-83-108-43 20-82 47-40-37-88-64 7-51 8-102-59-28-123-42-26 43-46 89-49-7-98 0-20-46-46-89-64 14-123 42 1 51 8 102-48 27-88 64-39-27-82-47-48 49-83 108 26 42 56 81zm0 33v39c0 276 813 276 814 0v-39l-134 12-5 69q-2 10-14 13l-162 11q-12 0-16-11l-10-65H446l-10 65q-4 11-16 11l-162-11q-12-3-14-13l-5-69z" fill="#478cbf"/><path d="M483 600c0 34 58 34 58 0v-86c0-34-58-34-58 0z"/><circle cx="725" cy="526" r="90"/><circle cx="299" cy="526" r="90"/></g><g fill="#414042"><circle cx="307" cy="532" r="60"/><circle cx="717" cy="532" r="60"/></g></g></svg>
              �UwA%X   res://Test 1/control.tscn;�'��� /   res://icon.svg             ECFG      application/config/name         USB Read Write     application/run/main_scene$         res://Test 1/control.tscn      application/config/features$   "         4.2    Forward Plus       application/config/icon         res://icon.svg               