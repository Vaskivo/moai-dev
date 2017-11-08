// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIDIALOGSDL_H
#define	MOAIDIALOGSDL_H

#include <moai-core/headers.h>
#include <SDL.h>

//================================================================//
// MOAIDialogSDL
//================================================================//
/**	@lua	MOAIDialogSDL
	@text 	Dialog controller for deskop platforms
*/
class MOAIDialogSDL :
	public ZLContextClass < MOAIDialogSDL, MOAILuaObject > {
private:

	SDL_Window*		mWindow;

	//----------------------------------------------------------------//
	static int		_show					( lua_State* L );

public:
	
	DECL_LUA_SINGLETON ( MOAIDialogSDL )
	
	GET_SET ( SDL_Window*, Window, mWindow )
	
	//----------------------------------------------------------------//
					MOAIDialogSDL			();
					~MOAIDialogSDL			();
	void			RegisterLuaClass		( MOAILuaState& state );
};

#endif
