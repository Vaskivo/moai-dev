----------------------------------------------------------------
-- Copyright (c) 2010-2017 Zipline Games, Inc.
-- All Rights Reserved.
-- http://getmoai.com
----------------------------------------------------------------

MOAISim.openWindow ( "test", 320, 480 )

viewport = MOAIViewport.new ()
viewport:setSize ( 320, 480 )
viewport:setScale ( 320, 480 )

layer = MOAIPartitionViewLayer.new ()
layer:setViewport ( viewport )
layer:pushRenderPass ()

gfxQuad = MOAISpriteDeck2D.new ()
gfxQuad:setTexture ( "moai.png" )
gfxQuad:setRect ( -64, -64, 64, 64 )

prop = MOAIProp.new ()
prop:setDeck ( gfxQuad )
prop:setLoc ( 0, 80 )
prop:setPartition ( layer )

font = MOAIFont.new ()
font:loadFromTTF ( "arialbd.ttf", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.?!", 12, 163 )

textbox = MOAITextLabel.new ()
textbox:setFont ( font )
textbox:setRect ( -160, -80, 160, 80 )
textbox:setLoc ( 0, -100 )
textbox:setYFlip ( true )
textbox:setAlignment ( MOAITextLabel.CENTER_JUSTIFY )
textbox:setPartition ( layer )

textbox:setString ( "Moai has installed correctly! <c:0F0>Check out the samples folder.<c>" )
textbox:spool ()

function twirlingTowardsFreedom ()
	while true do
		MOAICoroutine.blockOnAction ( prop:moveRot ( 0, 0, 360, 3 ) )
		MOAICoroutine.blockOnAction ( prop:moveRot ( 0, 0, -360, 3 ) )
	end
end

thread = MOAICoroutine.new ()
thread:run ( twirlingTowardsFreedom )
