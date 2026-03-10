@tool
extends Node

func _ready():	
	$Player.Initialize()
	
	#Load static objects
	$Lamp.load_model("res://Assets/field/model/t1/t1_ar/t1_ar001", "t1_ar001.trmdl")
	$Landscape/Enkei.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_landscape1.trmdl")
	$Landscape/Enkei2.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_landscape2.trmdl")
	$Mountains/Enkei.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_mountains1.trmdl")
	$Mountains/Enkei2.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_mountains2.trmdl")
	$Mountains/Range.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_mountain_range.trmdl")
	$Sky/Sky1.load_model("res://Assets/field/model/env/sky/sky1", "sky1.trmdl")
