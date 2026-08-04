@tool
extends Node

func _ready():		
	#Load static objects
	var land_enkei = TrinityModel.new()
	land_enkei.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_landscape1.trmdl")
	$Landscape.add_child(land_enkei)
	var land_enkei2 = TrinityModel.new()
	land_enkei2.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_landscape2.trmdl")
	$Landscape.add_child(land_enkei2)
	
	var mtn_enkei = TrinityModel.new()
	mtn_enkei.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_mountains1.trmdl")
	$Mountains.add_child(mtn_enkei)
	var mtn_enkei2 = TrinityModel.new()
	mtn_enkei2.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_mountains2.trmdl")
	$Mountains.add_child(mtn_enkei2)
	var mtn_range = TrinityModel.new()
	mtn_range.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_mountain_range.trmdl")
	$Mountains.add_child(mtn_range)
	
	var city = TrinityModel.new()
	city.load_model("res://Assets/field/model/t1/t1_ev/t1_ev001/", "t1_ev001_city.trmdl")
	$City.add_child(city)
	var water = TrinityModel.new()
	water.load_model("res://Assets/field/model/t1/t1_ev/t1_ev001/", "t1_ev001_water.trmdl")
	$City.add_child(water)
	
	var prism = TrinityModel.new()
	prism.load_model("res://Assets/field/model/t1/t1_b/t1_b_prism/", "t1_b_prism_bottom.trmdl")
	$Tower.add_child(prism)
	var prism2 = TrinityModel.new()
	prism2.load_model("res://Assets/field/model/t1/t1_b/t1_b_prism/", "t1_b_prism_main.trmdl")
	$Tower.add_child(prism2)
	var prism3 = TrinityModel.new()
	prism3.load_model("res://Assets/field/model/t1/t1_b/t1_b_prism/", "t1_b_prism_top.trmdl")
	$Tower.add_child(prism3)
	var prism4 = TrinityModel.new()
	prism4.load_model("res://Assets/field/model/t1/t1_b/t1_b_prism/", "t1_b_prism_lod.trmdl")
	$Tower.add_child(prism4)
