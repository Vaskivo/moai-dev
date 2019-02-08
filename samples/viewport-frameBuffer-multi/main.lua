----------------------------------------------------------------
-- Copyright (c) 2010-2017 Zipline Games, Inc.
-- All Rights Reserved.
-- http://getmoai.com
----------------------------------------------------------------

MOAISim.openWindow ( "test", 576, 576 )

-- set up offscreen drawing
local frameBuffer = MOAIFrameBufferTexture.new ()
frameBuffer:init ( 512, 512 )
--frameBuffer:setClearColor ()
MOAIRenderMgr.setBufferTable ({ frameBuffer })

gfxQuad = MOAISpriteDeck2D.new ()
gfxQuad:setTexture ( frameBuffer )
gfxQuad:setRect ( 0, 0, 512, 512 )
gfxQuad:setUVRect ( 0, 1, 1, 0 )

viewport = MOAIViewport.new ()
viewport:setSize ( 576, 576 )
viewport:setScale ( 576, -576 )

mainLayer = MOAIPartitionViewLayer.new ()
mainLayer:setViewport ( viewport )
MOAISim.pushRenderPass ( mainLayer )

mainProp = MOAIProp.new ()
mainProp:setDeck ( gfxQuad )
mainProp:setPiv ( 256, 256 )
mainProp:setRot ( 10 )
mainProp:setPartition ( mainLayer )

-- add layers to offscreen buffer
local function makeLayer ( x, y, w, h, r, g, b, a )

	local viewport = MOAIViewport.new ()
	viewport:setSize ( x, y, x + w, y + h )
	viewport:setScale ( w, -h )

	local layer = MOAIPartitionViewLayer.new ()
	layer:setViewport ( viewport )
	layer:setClearColor ( r, g, b, a )

	return layer
end

gfxQuad = MOAISpriteDeck2D.new ()
gfxQuad:setTexture ( "moai.png" )
gfxQuad:setRect ( -64, -64, 64, 64 )
gfxQuad:setUVRect ( 0, 0, 1, 1 )

local function makeProp ( x, y, xs, ys, r )
	local prop = MOAIProp.new ()
	prop:setDeck ( gfxQuad )
	prop:setLoc ( x, y )
	prop:setScl ( xs, ys )
	return prop
end

local layers = {}

table.insert ( layers, makeLayer ( 0, 0, 256, 256, 1, 0, 0, 1 ))
table.insert ( layers, makeLayer ( 256, 0, 256, 256, 0, 1, 0, 1 ))
table.insert ( layers, makeLayer ( 0, 256, 256, 256, 0, 0, 1, 1 ))
table.insert ( layers, makeLayer ( 256, 256, 256, 256, 0, 1, 1, 1 ))

frameBuffer:setRenderTable ( layers )

for i, layer in ipairs ( layers ) do
	makeProp ( 0, 0, 1, 1 ):setPartition ( layer )
end

function clickCallback ( down )

	if down then

		local mouseX, mouseY = MOAIInputMgr.device.pointer:getLoc ()
		mouseX, mouseY = mainLayer:wndToWorld ( mouseX, mouseY )

		if not mainProp:inside ( mouseX, mouseY ) then return end

		mouseX, mouseY = mainProp:worldToModel ( mouseX, mouseY )

		local hit = false

		for i, layer in ipairs ( layers ) do

			local x, y = layer:wndToWorld ( mouseX, mouseY )
			local pick = layer:getPartition ():propForPoint ( x, y )

			if pick then
				hit = true
				pick:moveRot ( 10, 0.125, MOAIEaseType.EASE_IN )
			end
		end

		if not hit then
			mainProp:moveRot ( 5, 0.125, MOAIEaseType.EASE_IN )
		end
	end
end

MOAIInputMgr.device.mouseLeft:setCallback ( clickCallback )
