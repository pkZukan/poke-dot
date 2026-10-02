extends Control

const TITLE_ARCHIVE_PATH := "res://Assets/ui/data/title_menu/title_menu_00_eng.arc"

var _layout_size := Vector2.ZERO
var _logo_bounds := Rect2()

@onready var title_rect: TextureRect = $Backdrop/Title

func _ready() -> void:
	Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
	
	#load SARC and combined bntx
	var archive := ResourceLoader.load(TITLE_ARCHIVE_PATH) as SeadArchive
	var textures := BinaryTextureArchive.new()
	var error := textures.LoadFromBuffer(archive.get_file_data("timg/__Combined.bntx"))
	if error != OK:
		push_error("Failed to load title textures: %s" % error_string(error))
		return

	var img_name = "grptext_title_logo_00^u"
	var image := textures.GetTexture(img_name)
	if image == null or image.is_empty():
		push_error("Title texture is missing or empty: %s" % img_name)
		return
	title_rect.texture = ImageTexture.create_from_image(image)
	_load_logo_layout(archive)

func _load_logo_layout(archive: SeadArchive) -> void:
	var layout_file := "blyt/title_menu_00.bflyt"
	if not archive.has_file(layout_file):
		push_error("Title archive is missing %s" % layout_file)
		return
	var bflyt := BinaryLayout.new()
	if bflyt.LoadFromBuffer(archive.get_file_data(layout_file)) != OK:
		push_error("Failed to load title layout")
		return
	var layout := bflyt.get_layout()
	var panes: Array = layout["panes"]
	for index in panes.size():
		var pane: Dictionary = panes[index]
		if pane["name"] != "P_t_logo_00":
			continue
			
		var pane_size: Vector2 = pane["size"]
		var bounds := Rect2(-pane_size * 0.5, pane_size)
		var parent_index := index
		while parent_index >= 0:
			var ancestor: Dictionary = panes[parent_index]
			if ancestor["origin"] != 0 or ancestor["rotation"] != Vector3.ZERO:
				push_error("Unsupported title logo pane transform")
				return
			var translation: Vector3 = ancestor["translation"]
			var pane_scale: Vector2 = ancestor["scale"]
			bounds.position = bounds.position * pane_scale + Vector2(translation.x, -translation.y)
			bounds.size *= pane_scale
			parent_index = ancestor["parent"]
		_layout_size = layout["size"]
		if layout["draw_from_center"]:
			bounds.position += _layout_size * 0.5
		_logo_bounds = bounds
		$Backdrop.resized.connect(_update_logo_rect)
		_update_logo_rect()
		return
	push_error("Title layout is missing P_t_logo_00")

func _update_logo_rect() -> void:
	if _layout_size.x <= 0.0 or _layout_size.y <= 0.0:
		return
	var available: Vector2 = $Backdrop.size
	var fit_scale := minf(available.x / _layout_size.x, available.y / _layout_size.y)
	var margin := (available - _layout_size * fit_scale) * 0.5
	title_rect.position = margin + _logo_bounds.position * fit_scale
	title_rect.size = _logo_bounds.size * fit_scale

func NewGame():
	GameManager.state = GameManager.GameState.STATE_PLAYING
	GameManager.LoadScene("main.tscn")
	
func LoadGame():
	pass
	
func QuitGame():
	get_tree().quit()
