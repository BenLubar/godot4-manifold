extends Node3D

func _ready() -> void:
	var box: MeshInstance3D = %Box
	box.mesh = ManifoldMesh.cube(Vector3.ONE, true).rotate(Vector3(45, 0, 0)).to_mesh()
