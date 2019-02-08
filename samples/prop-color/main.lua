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
gfxQuad:setTexture ( "../resources/moai.png" )
gfxQuad:setRect ( -64, -64, 64, 64 )

prop = MOAIProp.new ()
prop:setDeck ( gfxQuad )
prop:setColor ( 0, 0, 0, 0 )
prop:seekColor ( 1, 0, 0, 1, 1.5 )
prop:setPartition ( layer )

prop:moveRot ( 0, 0, 360, 1.5 )

