Player = {}

function Player.setup()
	local obj = {}

	return obj
end

spriteRenderer = {}
currScene = {}
cameraObject = {}
sceneClearColor = {}
object = {}

function Player.initialize(self)
	if (self.owner:hasSpriteRenderer()) then
		spriteRenderer = self.owner:getSpriteRenderer()
	end

	currScene = self.owner:scene()
	cameraObject = currScene:findEntity("Camera")
	sceneClearColor = currScene:clearColor()
	object = currScene:findEntity("Object")
end

function Player.update(self, dt)
	if (sceneClearColor.r >= 255) then
		sceneClearColor.r = 0
	end

	sceneClearColor.r = sceneClearColor.r + 1

	self.owner:lookAt(getMousePositionX(), getMousePositionY())

	if (isMouseButton(0)) then
		self.owner:transform():increasePosition(-math.cos(math.rad(self.owner:transform().rotation)), -math.sin(math.rad(self.owner:transform().rotation)))
	end

	cameraObject:transform():increasePosition(getHorizontalAxis(), getVerticalAxis())

	if (isMouseButtonDown(1)) then
		newObject = object:copy("Object1")
		newObject:transform().position = self.owner:transform().position
	end

	if (isKeyDown(256)) then
		closeApplication()
	end
end
