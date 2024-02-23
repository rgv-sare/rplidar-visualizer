-- Called upon scene starting up
function Start()
  scene.camera.transform:setPosition({0, 0, -1})
  scene.camera:setProjectionMode("ORTHOGRAPHIC")
  scene.camera:setOrthographicSize(2)
end

-- Called upon every frame
function Update(dt)
  
end