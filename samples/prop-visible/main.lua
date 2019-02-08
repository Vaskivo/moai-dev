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

prop1 = MOAIProp.new ()
prop1:setDeck ( gfxQuad )
prop1:setPartition ( layer )

prop2 = MOAIProp.new ()
prop2:setDeck ( gfxQuad )
prop2:setLoc(10, 10)
prop2:setAttrLink ( MOAIProp.INHERIT_VISIBLE, prop1, MOAIProp.ATTR_VISIBLE )
prop2:setPartition ( layer )

prop3 = MOAIProp.new ()
prop3:setDeck ( gfxQuad )
prop3:setLoc(20, 20)
prop3:setAttrLink ( MOAIProp.INHERIT_VISIBLE, prop2, MOAIProp.ATTR_VISIBLE )
prop3:setPartition ( layer )

prop4 = MOAIProp.new ()
prop4:setDeck ( gfxQuad )
prop4:setLoc(30, 30)
prop4:setAttrLink ( MOAIProp.ATTR_LOCAL_VISIBLE, prop3, MOAIProp.ATTR_VISIBLE )
prop4:setPartition ( layer )

function printVisibleState(propName)
    local prop = _G[propName]
    print(propName .. ":", "LOCAL_VISIBLE=" .. prop:getAttr( MOAIProp.ATTR_LOCAL_VISIBLE ), "ATTR_VISIBLE=" .. prop:getAttr( MOAIProp.ATTR_VISIBLE )  )
end

function printVisibleStates()
    print("----------------------------------------")
    printVisibleState("prop1")
    printVisibleState("prop2")
    printVisibleState("prop3")
    printVisibleState("prop4")
end

-- visible test
prop1:setVisible(false)
printVisibleStates()

prop2:setVisible(false)
printVisibleStates()

prop1:forceUpdate()
printVisibleStates()

prop1:setVisible(true)
printVisibleStates()

prop1:forceUpdate()
printVisibleStates()

prop4:forceUpdate()
printVisibleStates()

prop4:setVisible(true)
printVisibleStates()
