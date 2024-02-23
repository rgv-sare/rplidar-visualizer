-- Called upon scene starting up
function Start()
  scene.camera.transform:setPosition({0, 0, -5})
  scene.camera.transform:setRotationEuler({0, 0, 0})
  scene.camera:setProjectionMode("PERSPECTIVE")
  scene.camera:setFieldOfView(25)
end

-- Called upon every frame
function Update(dt)
  
end