// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAICamera.h>
#include <moai-sim/MOAICollisionShape.h>
#include <moai-sim/MOAICollisionWorld.h>
#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIDebugLines.h>
#include <moai-sim/MOAIDraw.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAILayoutFrame.h>
#include <moai-sim/MOAIRenderMgr.h>
#include <moai-sim/MOAIPartition.h>
#include <moai-sim/MOAIPartitionResultMgr.h>
#include <moai-sim/MOAIPartitionResultBuffer.h>
#include <moai-sim/MOAICollisionProp.h>
#include <moai-sim/MOAIScissorRect.h>
#include <moai-sim/MOAIShader.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAISurfaceSampler2D.h>
#include <moai-sim/MOAITexture.h>
#include <moai-sim/MOAITextureBase.h>
#include <moai-sim/MOAIViewport.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
int MOAICollisionProp::_collisionMove ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAICollisionProp, "U" )
	
	ZLVec3D move = state.GetValue < ZLVec3D >( 2, ZLVec3D::ORIGIN );
	
	self->Move ( move );
	
	return 0;
}

//----------------------------------------------------------------//
// TODO: doxygen
int MOAICollisionProp::_getOverlaps ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAICollisionProp, "U" )

	u32 total = 0;
	MOAIPropOverlapLink* link = self->mOverlapLinks;
	for ( ; link; link = link->mNext, ++total ) {
		state.Push ( link->mOtherLink->mProp );
	}
	return total;
}

//----------------------------------------------------------------//
// TODO: doxygen
int MOAICollisionProp::_hasOverlaps ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAICollisionProp, "U" )

	return self->mOverlapLinks != 0;
}

//----------------------------------------------------------------//
// TODO: doxygen
//int MOAICollisionProp::_setGroupMask ( lua_State* L ) {
//	MOAI_LUA_SETUP ( MOAICollisionProp, "U" )
//	
//	self->mGroupMask = state.GetValue < u32 >( 2, 0 );
//	return 0;
//}

//----------------------------------------------------------------//
// TODO: doxygen
int MOAICollisionProp::_setOverlapFlags ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAICollisionProp, "U" )
	
	self->mOverlapFlags = state.GetValue < u32 >( 2, 0 );
	return 0;
}

//================================================================//
// MOAICollisionProp
//================================================================//

//----------------------------------------------------------------//
void MOAICollisionProp::ClearOverlapLink ( MOAIPropOverlap& overlap ) {

	MOAIPropOverlapLink* cursor = this->mOverlapLinks;
	this->mOverlapLinks = 0;
	
	while ( cursor ) {
		MOAIPropOverlapLink* overlapLink = cursor;
		cursor = cursor->mNext;
		
		if ( overlapLink->mOverlap != &overlap ) {
			overlapLink->mNext = this->mOverlapLinks;
			this->mOverlapLinks = overlapLink;
		}
	}
}

//----------------------------------------------------------------//
void MOAICollisionProp::DrawContactPoints ( MOAIDrawShape& draw, const MOAIContactPoint2D* contacts, u32 nContacts ) {

	draw.SetPenWidth ( 1.0f );

	for ( u32 i = 0; i < nContacts; ++i ) {
	
		const MOAIContactPoint2D& contact = contacts [ i ];
	
		ZLVec3D point ( contact.mPoint.mX, contact.mPoint.mY, 0.0f );
		ZLVec3D normal ( contact.mNormal.mX, contact.mNormal.mY, 0.0f );
		ZLVec3D cornerTangent ( contact.mCornerTangent.mX, contact.mCornerTangent.mY, 0.0f );
	
		draw.SetPenColor ( ZLColor::PackRGBA ( 0.0f, 1.0f, 0.0f, 1.0f ));
		draw.DrawRay ( point.mX, point.mY, normal.mX, normal.mY, 32.0f );
		
		switch ( contact.mType ) {
		
			case MOAIContactPoint2D::LEAVING:

				draw.SetPenColor ( ZLColor::PackRGBA ( 1.0f, 0.0f, 1.0f, 1.0f ));
				break;
				
			case MOAIContactPoint2D::CROSSING:

				draw.SetPenColor ( ZLColor::PackRGBA ( 1.0f, 0.0f, 0.0f, 1.0f ));
				break;
				
			case MOAIContactPoint2D::PARALLEL:

				draw.SetPenColor ( ZLColor::PackRGBA ( 0.0f, 1.0f, 1.0f, 1.0f ));
				break;
				
			case MOAIContactPoint2D::CORNER:
				
				draw.SetPenColor ( ZLColor::PackRGBA ( 0.85f, 0.0f, 1.0f, 1.0f ));
				draw.DrawRay ( point.mX, point.mY, cornerTangent.mX, cornerTangent.mY, 48.0f );
			
				draw.SetPenColor ( ZLColor::PackRGBA ( 1.0f, 1.0f, 0.0f, 1.0f ));
				break;
		}
		
		draw.DrawRectFill ( point.mX - 4.0f, point.mY + 4.0f, point.mX + 4.0f, point.mY - 4.0f );
	}
}

//----------------------------------------------------------------//
void MOAICollisionProp::FindContactPoints ( MOAIContactPointAccumulator2D& accumulator, MOAICollisionProp& other ) {

	MOAICollisionShape* shape0 = this->GetCollisionShape ();
	MOAICollisionShape* shape1 = other.GetCollisionShape ();
	
	if ( shape0 && shape1 ) {
	
		shape0->FindContactPoints ( accumulator, *shape1, *this, other );
	}
}

//----------------------------------------------------------------//
void MOAICollisionProp::FindOverlapInterval ( MOAIVectorAccumulator& accumulator, MOAICollisionProp& other ) {

	MOAICollisionShape* shape0 = this->GetCollisionShape ();
	MOAICollisionShape* shape1 = other.GetCollisionShape ();
	
	if ( shape0 && shape1 ) {
	
		shape0->FindOverlapInterval ( accumulator, *shape1, *this, other );
	}
}

//----------------------------------------------------------------//
void MOAICollisionProp::GatherContactPoints ( MOAIContactPointAccumulator2D& accumulator, const ZLBox& worldBounds ) {

	accumulator.Reset ();

	MOAICollisionWorld& world = *this->mCollisionWorld;

	MOAIPartitionResultBuffer& buffer = MOAIPartitionResultMgr::Get ().GetBuffer ();
	u32 interfaceMask = world.GetInterfaceMask < MOAICollisionProp >();
	u32 totalResults = world.GatherHulls ( buffer, this, worldBounds, interfaceMask );
	
	for ( u32 i = 0; i < totalResults; ++i ) {
		MOAIPartitionResult* result = buffer.GetResultUnsafe ( i );
		MOAICollisionProp* otherProp = result->AsType < MOAICollisionProp >();
		if ( !otherProp ) continue;
		this->FindContactPoints ( accumulator, *otherProp );
	}
}

//----------------------------------------------------------------//
MOAICollisionShape* MOAICollisionProp::GetCollisionShape () {

	return this->mDeck ? this->mDeck->GetCollisionShape ( this->mIndex - 1 ) : 0;
}

//----------------------------------------------------------------//
bool MOAICollisionProp::IsActive () {

	return this->mActiveListLink.List () != 0;
}

//----------------------------------------------------------------//
MOAICollisionProp::MOAICollisionProp () :
	mCategory ( CATEGORY_MASK_ALL ),
	mMask ( CATEGORY_MASK_ALL ),
	mOverlapFlags ( DEFAULT_OVERLAP_FLAGS ),
	mOverlapPass ( MOAICollisionWorld::OVERLAP_PASS_INIT ),
	mOverlapLinks ( 0 ),
	mStayActive ( false ),
	mTouched ( MOAICollisionWorld::OVERLAP_PASS_INIT ),
	mCollisionWorld ( 0 ) {

	RTTI_BEGIN
		RTTI_EXTEND ( MOAIPartitionHull )
		RTTI_EXTEND ( MOAIDrawable )
		RTTI_EXTEND ( MOAIIndexedPropBase )
	RTTI_END
	
	this->mActiveListLink.Data ( this );
}

//----------------------------------------------------------------//
MOAICollisionProp::~MOAICollisionProp () {
}

//----------------------------------------------------------------//
void MOAICollisionProp::Move ( ZLVec3D move ) {

	// just cramp everytning in here for now.
	// possible to break into steps for collision world later *if*
	// more traditional physics approach is desired.
	
	// purpose here is just a proof of concept for an
	// edge-following algorithm. will make all this user-configurable
	// later.

	static u32 MAX_PASSES = 8;

	MOAICollisionWorld& world = *this->mCollisionWorld;
	MOAIDrawShapeRetained& draw = *this->mCollisionWorld;
	draw.SetPenWidth ( 1.0f );

	MOAIPartitionResultBuffer& buffer = MOAIPartitionResultMgr::Get ().GetBuffer ();
	u32 interfaceMask = world.GetInterfaceMask < MOAICollisionProp >();
	u32 totalResults = 0;
	
	MOAIContactPoint2D* contacts = ( MOAIContactPoint2D* )alloca ( 128 * sizeof ( MOAIContactPoint2D ));
	MOAIContactPointAccumulator2D contactAccumulator ( contacts, 128 );
	
	ZLVec2D moveNorm ( move.mX, move.mY );
	float moveLength = moveNorm.NormSafe ();
	
	if ( moveLength > EPSILON ) {
	
		for ( u32 i = 0; i < MAX_PASSES; ++i ) {
		
			// find best contact points
			
			float bestPushDot = 2.0f;
			const MOAIContactPoint2D* bestPushContact = 0;
			
			float bestPullDot = 2.0f;
			float farthestPull = 0.0f;
			const MOAIContactPoint2D* bestPullContact = 0;
		
			// find contacts
			ZLBox worldBounds = this->GetWorldBounds ();
			worldBounds.Inflate ( 10.0f ); // TODO: epsilon
		
			this->GatherContactPoints ( contactAccumulator, worldBounds );
			u32 nContacts = contactAccumulator.Top ();
			
			for ( u32 i = 0; i < nContacts; ++i ) {
				const MOAIContactPoint2D& contact = contacts [ i ];
				
				// ignore corner contacts if they are behind the move
				if ( contact.mType == MOAIContactPoint2D::CORNER ) {
					if ( moveNorm.Dot ( contact.mCornerTangent ) < -EPSILON ) continue;
					//if ( moveNorm.Dot ( contact.mCornerTangent ) < EPSILON ) continue;
				}
				
				float d = moveNorm.Dot ( contact.mNormal );
				
				if ( d <= EPSILON ) {

					// heading into - push
					if ( d < bestPushDot ) {
						bestPushContact = &contact;
						bestPushDot = d;
					}
				}
				else {

//					// heading out of - pull
//					if ( d < bestPullDot ) {
//						bestPullContact = &contact;
//						bestPullDot = d;
//					}

					bool take = false;
					float dist = moveNorm.Dot ( contact.mPoint );
					
					if ( !bestPullContact ) {
					
						take = true;
					}
					else {
					
						if ( farthestPull < ( dist + EPSILON )) {
						
							take = true;
						
						}
						else if ( ABS ( farthestPull - dist ) < EPSILON ) {
						
							take = ( d < bestPullDot );
						}
					}
					
					// heading out of - pull
					if ( take ) {
						bestPullContact = &contact;
						farthestPull = dist;
						bestPullDot = d;
					}
				}
			}

			const MOAIContactPoint2D* bestContact = 0;

			if ( bestPushContact ) {

				// straight into a wall
				if ( bestPushDot <= ( EPSILON - 1.0f )) {
					break;
				}
				bestContact = bestPushContact;
			}
			else if ( bestPullContact ) {
			
				// straight out of a wall
				if ( bestPullDot >= ( 1.0f - EPSILON )) {
					break;
				}
				bestContact = bestPullContact;
			}

//			if ( nContacts && !bestContact ) {
//				printf ( "FLY FLY!\n" );
//			}

			ZLVec2D stepMoveNorm;
			float stepMoveLength;

			if ( bestContact ) {
						
				stepMoveNorm.Init ( move.mX, move.mY );
				stepMoveNorm.PerpProject ( bestContact->mNormal );
				stepMoveNorm.Norm ();
				
				float maxMove = stepMoveNorm.Dot ( bestContact->mTangent ) > 0.0f ? bestContact->mPosD : bestContact->mNegD;
				stepMoveLength = moveLength < maxMove ? moveLength : maxMove;
				
				printf ( "----> STEP: (%g, %g) EDGE (%g, %g) MAX MOVE: %g\n",
					move.mX, move.mY,
					bestContact->mTangent.mX, bestContact->mTangent.mY,
					maxMove
				);
				printf ( "\n" );
			}
			else {
			
				stepMoveNorm.Init ( moveNorm.mX, moveNorm.mY );
				stepMoveLength = moveLength;
			}
			
			// TODO: final check that we aren't moving into a wall

			// move (and force a dep node update)
			// TODO: this needs to be *way* more efficient!
			this->mLoc.mX += stepMoveNorm.mX * stepMoveLength;
			this->mLoc.mY += stepMoveNorm.mY * stepMoveLength;
			this->MOAITransformBase::MOAINode_Update ();
			this->MOAIPartitionHull::MOAINode_Update ();

			// prepare the next step or break

			moveLength = moveLength - stepMoveLength;
			if ( moveLength < EPSILON ) break;
		}
	}
	
	MOAICollisionProp::DrawContactPoints ( *this->mCollisionWorld, contacts, contactAccumulator.Top ());
	
	// resolve overlaps
	totalResults = world.GatherHulls ( buffer, this, this->GetWorldBounds (), interfaceMask );
	
	MOAIVectorAccumulator accumulator;
	
	for ( u32 i = 0; i < totalResults; ++i ) {
		MOAIPartitionResult* result = buffer.GetResultUnsafe ( i );
		MOAICollisionProp* otherProp = result->AsType < MOAICollisionProp >();
		if ( !otherProp ) continue;
		this->FindOverlapInterval ( accumulator, *otherProp );
	}
	ZLVec3D resolveOverlaps = accumulator.GetAverage ();
	this->mLoc.Add ( resolveOverlaps );
	this->ScheduleUpdate ();

	if ( move.LengthSqrd () && this->mCollisionWorld ) {
			
		draw.SetPenColor ( ZLColor::PackRGBA ( 1.0f, 1.0f, 0.0f, 1.0f ));
		draw.DrawCircleOutline ( this->mLoc.mX, this->mLoc.mY, 32.0f, 16 );
		
		move.Norm ();
		move.Scale ( 32.0f );
		draw.DrawLine ( this->mLoc.mX, this->mLoc.mY, this->mLoc.mX + move.mX, this->mLoc.mY + move.mY );
	}
}

//----------------------------------------------------------------//
void MOAICollisionProp::RegisterLuaClass ( MOAILuaState& state ) {
	
	MOAIPartitionHull::RegisterLuaClass ( state );
	MOAIIndexedPropBase::RegisterLuaClass ( state );
	
	MOAIDebugLinesMgr::Get ().ReserveStyleSet < MOAICollisionProp >( TOTAL_DEBUG_LINE_STYLES );
	
	state.SetField ( -1, "DEBUG_DRAW_COLLISION_PROP_MASTER",					MOAIDebugLinesMgr::Pack < MOAICollisionProp >( -1 ));
	state.SetField ( -1, "DEBUG_DRAW_COLLISION_ACTIVE_PROP_BOUNDS",				MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_ACTIVE_PROP_BOUNDS ));
	state.SetField ( -1, "DEBUG_DRAW_COLLISION_ACTIVE_OVERLAP_PROP_BOUNDS",		MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_ACTIVE_OVERLAP_PROP_BOUNDS ));
	state.SetField ( -1, "DEBUG_DRAW_COLLISION_ACTIVE_TOUCHED_PROP_BOUNDS",		MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_ACTIVE_TOUCHED_PROP_BOUNDS ));
	state.SetField ( -1, "DEBUG_DRAW_COLLISION_OVERLAP_PROP_BOUNDS",			MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_OVERLAP_PROP_BOUNDS ));
	state.SetField ( -1, "DEBUG_DRAW_COLLISION_OVERLAPS",						MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_OVERLAPS ));
	state.SetField ( -1, "DEBUG_DRAW_COLLISION_WORLD_BOUNDS",					MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_WORLD_BOUNDS ));
	
	state.SetField ( -1, "OVERLAP_EVENTS_ON_UPDATE",		( u32 )OVERLAP_EVENTS_ON_UPDATE );
	state.SetField ( -1, "OVERLAP_EVENTS_CONTINUOUS",		( u32 )OVERLAP_EVENTS_CONTINUOUS );
	state.SetField ( -1, "OVERLAP_EVENTS_LIFECYCLE",		( u32 )OVERLAP_EVENTS_LIFECYCLE );
	state.SetField ( -1, "OVERLAP_GRANULARITY_FINE",		( u32 )OVERLAP_GRANULARITY_FINE );
	state.SetField ( -1, "OVERLAP_CALCULATE_BOUNDS",		( u32 )OVERLAP_CALCULATE_BOUNDS );
	
	state.SetField ( -1, "CATEGORY_MASK_ALL",				( u32 )CATEGORY_MASK_ALL );
}

//----------------------------------------------------------------//
void MOAICollisionProp::RegisterLuaFuncs ( MOAILuaState& state ) {
	
	MOAIPartitionHull::RegisterLuaFuncs ( state );
	MOAIIndexedPropBase::RegisterLuaFuncs ( state );
	
	luaL_Reg regTable [] = {
		{ "collisionMove",		_collisionMove },
		{ "getOverlaps",		_getOverlaps },
		{ "hasOverlaps",		_hasOverlaps },
		//{ "setGroupMask",		_setGroupMask },
		{ "setOverlapFlags",	_setOverlapFlags },
		{ NULL, NULL }
	};
	
	luaL_register ( state, 0, regTable );
}

//----------------------------------------------------------------//
void MOAICollisionProp::SerializeIn ( MOAILuaState& state, MOAIDeserializer& serializer ) {
	
	MOAIPartitionHull::SerializeIn ( state, serializer );
	MOAIIndexedPropBase::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAICollisionProp::SerializeOut ( MOAILuaState& state, MOAISerializer& serializer ) {
	
	MOAIPartitionHull::SerializeOut ( state, serializer );
	MOAIIndexedPropBase::SerializeOut ( state, serializer );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAICollisionProp::MOAIDrawable_DrawDebug ( int subPrimID ) {
	UNUSED ( subPrimID );

	MOAIDebugLinesMgr& debugLines = MOAIDebugLinesMgr::Get ();
	if ( !( debugLines.IsVisible () && debugLines.SelectStyleSet < MOAICollisionProp >())) return;

	MOAIGfxMgr& gfxMgr = MOAIGfxMgr::Get ();
	
	MOAIDraw& draw = MOAIDraw::Get ();
	UNUSED ( draw ); // mystery warning in vs2008
	
	draw.Bind ();

	if ( debugLines.Bind ( MOAICollisionProp::DEBUG_DRAW_COLLISION_WORLD_BOUNDS )) {
		gfxMgr.mVertexCache.SetVertexTransform ( gfxMgr.mGfxState.GetMtx ( MOAIGfxGlobalsCache::WORLD_TO_CLIP_MTX ));
		draw.DrawBoxOutline ( this->GetWorldBounds ());
	}
	
	MOAICollisionShape* shape = this->GetCollisionShape ();
		
	if ( shape ) {
		const ZLAffine3D& localToWorldMtx = this->GetLocalToWorldMtx ();
		gfxMgr.mGfxState.SetMtx ( MOAIGfxGlobalsCache::MODEL_TO_WORLD_MTX, localToWorldMtx );
		shape->Draw ( localToWorldMtx );
	}

	bool visible = false;
	
	if ( this->IsActive ()) {
		
		if ( this->mOverlapLinks ) {
			visible = debugLines.Bind ( DEBUG_DRAW_COLLISION_ACTIVE_OVERLAP_PROP_BOUNDS );
		}
		
		if ( this->mTouched == this->mOverlapPass && !visible ) {
			visible = debugLines.Bind ( DEBUG_DRAW_COLLISION_ACTIVE_TOUCHED_PROP_BOUNDS );
		}
		
		if ( !visible ) {
			visible = debugLines.Bind ( DEBUG_DRAW_COLLISION_ACTIVE_PROP_BOUNDS );
		}
	}
	
	if ( this->mOverlapLinks && !visible ) {
		visible = debugLines.Bind ( DEBUG_DRAW_COLLISION_OVERLAP_PROP_BOUNDS );
	}
	
	if ( visible ) {
		
		MOAICollisionShape* shape = this->GetCollisionShape ();
		
		if ( shape ) {
			const ZLAffine3D& localToWorldMtx = this->GetLocalToWorldMtx ();
			gfxMgr.mGfxState.SetMtx ( MOAIGfxGlobalsCache::MODEL_TO_WORLD_MTX, localToWorldMtx );
			shape->Draw ( localToWorldMtx );
		}
		else {
			gfxMgr.mVertexCache.SetVertexTransform ( gfxMgr.mGfxState.GetMtx ( MOAIGfxGlobalsCache::WORLD_TO_CLIP_MTX ));
			draw.DrawBoxOutline ( this->GetWorldBounds ());
		}
	}
	
	if ( debugLines.Bind ( MOAICollisionProp::DEBUG_DRAW_COLLISION_OVERLAPS )) {
	
		gfxMgr.mVertexCache.SetVertexTransform ( gfxMgr.mGfxState.GetMtx ( MOAIGfxGlobalsCache::WORLD_TO_CLIP_MTX ));

		MOAIPropOverlapLink* overlapLinkIt = this->mOverlapLinks;
		for ( ; overlapLinkIt; overlapLinkIt = overlapLinkIt->mNext ) {
			const ZLBounds& bounds = overlapLinkIt->mOverlap->mBounds;
			if ( bounds.mStatus == ZLBounds::ZL_BOUNDS_OK ) {
				draw.DrawBoxOutline ( bounds );
			}
		}
	}
}

//----------------------------------------------------------------//
bool MOAICollisionProp::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {

	if ( MOAIIndexedPropBase::MOAINode_ApplyAttrOp ( attrID, attr, op )) return true;
	if ( MOAIPartitionHull::MOAINode_ApplyAttrOp ( attrID, attr, op )) return true;
	return false;
}

//----------------------------------------------------------------//
void MOAICollisionProp::MOAINode_Update () {
	
	MOAIPartitionHull::MOAINode_Update ();
	
	if ( this->mCollisionWorld && this->mOverlapFlags ) {
		this->mCollisionWorld->MakeActive ( *this );
	}
}

//----------------------------------------------------------------//
void MOAICollisionProp::MOAIPartitionHull_AddToSortBuffer ( MOAIPartitionResultBuffer& buffer, u32 key ) {

	buffer.PushResult ( *this, key, NO_SUBPRIM_ID, this->GetPriority (), this->GetWorldLoc (), this->GetWorldBounds ());
}

//----------------------------------------------------------------//
u32 MOAICollisionProp::MOAIPartitionHull_AffirmInterfaceMask ( MOAIPartition& partition ) {

	return (
		partition.AffirmInterfaceMask < MOAICollisionProp >() |
		partition.AffirmInterfaceMask < MOAIDrawable >()
	);
}

//----------------------------------------------------------------//
ZLBounds MOAICollisionProp::MOAIPartitionHull_GetModelBounds () {

	MOAICollisionShape* shape = this->GetCollisionShape ();
	if ( shape ) {
		ZLBounds bounds;
		bounds.Init ( shape->GetBounds ());
		return bounds;
	}
	return ZLBounds::EMPTY;
}

//----------------------------------------------------------------//
bool MOAICollisionProp::MOAIPartitionHull_PrepareForInsertion ( const MOAIPartition& partition ) {
	UNUSED ( partition );
	return true;
}

//----------------------------------------------------------------//
void MOAICollisionProp::MOAIPartitionHull_WasRemovedFromPartition () {

	this->mCollisionWorld = 0;
}
