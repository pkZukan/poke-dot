@tool
extends Node

func _ready():		
	#Load static objects
	$Lamp.load_model("res://Assets/field/model/t1/t1_ar/t1_ar001", "t1_ar001.trmdl")
	$Landscape/Enkei.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_landscape1.trmdl")
	$Landscape/Enkei2.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_landscape2.trmdl")
	$Mountains/Enkei.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_mountains1.trmdl")
	$Mountains/Enkei2.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_mountains2.trmdl")
	$Mountains/Range.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_mountain_range.trmdl")
	
	var city = TrinityModel.new()
	city.load_model("res://Assets/field/model/t1/t1_ev/t1_ev001/", "t1_ev001_city.trmdl")
	$City.add_child(city)
	var water = TrinityModel.new()
	water.load_model("res://Assets/field/model/t1/t1_ev/t1_ev001/", "t1_ev001_water.trmdl")
	$City.add_child(water)
