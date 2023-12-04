#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "rcamera.h"

#define WINDOW_TITLE	"Voxel Test"
#define SCREEN_WIDTH	2048
#define SCREEN_HEIGHT	1024
#define FONT_SIZE		20

#define ROT_FACTOR		0.1f
#define MOV_FACTOR		0.2f

#define DEBUG			false

#define elif else if

#define XPDIR 0
#define XNDIR 1
#define ZPDIR 2
#define ZNDIR 3
#define YPDIR 4
#define YNDIR 5


#define XPCOL PINK
#define XNCOL RED
#define ZPCOL YELLOW
#define ZNCOL GREEN
#define YPCOL BLUE
#define YNCOL VIOLET

#define S 32

#define FNP		(void**)&
void nullFree	(void **ptrloc){ free( *ptrloc ); *ptrloc = 0; }

//#define FNA			(void***)&
//void nullFreeArray	(void ***aploc, int size){ for (int i = 0; i < size; i++){ nullFree( aploc[i] ); }}



Camera	*initPlayerCam();
void	generateGeometry( bool voxels[S][S][S] );

void	refreshView( Camera *camera );
void	drawGeometry( Camera *camera, bool voxels[S][S][S] );
void	drawInfoBox( Camera *camera );



int main(void)
{
	// Initialize the camera
	Camera *camera = initPlayerCam();

	bool voxels[S][S][S];

	generateGeometry( voxels );

	// Main game loop
	while( !WindowShouldClose() )	// Detect window close button or ESC key
	{
		refreshView(camera);

		// Draw phase
		BeginDrawing();

			ClearBackground( DARKGRAY );

			//drawGeometry( camera, heights, positions, colours );
			drawGeometry( camera, voxels );

			drawInfoBox( camera );

		EndDrawing();
	}

	//nullFree( FNP heights );
	//nullFree( FNP positions );
	//nullFree( FNP colours );

	CloseWindow();

	nullFree( FNP camera );

	return 0;
}



// Define the camera to look into our 3d world (position, target, up vector)
Camera *initPlayerCam()
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
	DisableCursor();	// Limit cursor to relative movement inside the window
	SetTargetFPS(60);

	Camera *camera = calloc( 1, sizeof( Camera ));

	camera->fovy		= 90.0f;
	camera->projection	= CAMERA_PERSPECTIVE;

	camera->position	= ( Vector3 ){ -10.0f, -10.0f, -10.0f };
	camera->target		= ( Vector3 ){ 0.0f, 0.0f, 0.0f };
	camera->up			= ( Vector3 ){ 0.0f, 1.0f, 0.0f };

	return camera;
}

bool 	shouldHaveVoxel( int x, int y, int z )
{
	if( x < 0 || x >= S ){ return false; }
	if( y < 0 || y >= S ){ return false; }
	if( z < 0 || z >= S ){ return false; }

	//if( rand() % 32 == 0 ){ return false; }

	if( x == S / 2 && y == S / 2 && z == S / 2 ){ return true; }

	int xDist2 = ( x - ( S / 2 )) * ( x - ( S / 2 ));
	int yDist2 = ( y - ( S / 2 )) * ( y - ( S / 2 ));
	int zDist2 = ( z - ( S / 2 )) * ( z - ( S / 2 ));
	int threshold = (( S / 2 ) * ( S / 2 ));

	if( xDist2 + yDist2 + zDist2 < threshold){ return false; }

	return true;
}

void	generateGeometry( bool voxels[S][S][S] )
{
	for( int x = 0; x < S; x++ )
		for( int y = 0; y < S; y++ )
			for( int z = 0; z < S; z++ )
				if( shouldHaveVoxel( x, y, z ))
					voxels[x][y][z] = true;
				else
					voxels[x][y][z] = false;
}

bool	hasVoxel( bool voxels[S][S][S], int x, int y, int z )
{
	if( x < 0 || x >= S ){ return false; }
	if( y < 0 || y >= S ){ return false; }
	if( z < 0 || z >= S ){ return false; }

	return voxels[x][y][z];
}



// This new camera function allows custom movement/rotation values to be directly provided
// as input parameters, with this approach, rcamera module is internally independent of raylib inputs
void	refreshView( Camera *camera )
{
	int run = 1;
	if( IsKeyDown( KEY_LEFT_CONTROL )){ run = 2; }

	UpdateCameraPro( camera,
		(Vector3){
			(IsKeyDown( KEY_W ) || IsKeyDown( KEY_UP ))		* MOV_FACTOR * run -	// Move forward-backward
			(IsKeyDown( KEY_S ) || IsKeyDown( KEY_DOWN ))	* MOV_FACTOR * run,

			(IsKeyDown( KEY_D ) || IsKeyDown( KEY_RIGHT ))	* MOV_FACTOR * run -	// Move right-left
			(IsKeyDown( KEY_A ) || IsKeyDown( KEY_LEFT ))	* MOV_FACTOR * run,

			(IsKeyDown( KEY_SPACE ) || IsKeyDown( KEY_RIGHT_CONTROL ))	* MOV_FACTOR * run -	// Move up-down
			(IsKeyDown( KEY_LEFT_SHIFT ) || IsKeyDown( KEY_KP_0 ))		* MOV_FACTOR * run
		},
		( Vector3 ){
			GetMouseDelta().x * ROT_FACTOR,	// Rotation: yaw
			GetMouseDelta().y * ROT_FACTOR,	// Rotation: pitch
			0.0f							// Rotation: roll
		},
		0.0f);	// Move to target (zoom)
}

void	drawFace( Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Color color )
{
	DrawTriangle3D( v1, v2, v3, color );
	DrawTriangle3D( v1, v3, v4, color );

	if( DEBUG ){
		DrawLine3D( v1, v2, BLACK );
		DrawLine3D( v1, v3, BLACK );
		DrawLine3D( v1, v4, BLACK );
		DrawLine3D( v2, v3, BLACK );
		DrawLine3D( v3, v4, BLACK );
	}
}

void	drawVoxel( Vector3 pos )
{
	Vector3 p0 = { pos.x + 0.0f, pos.y + 0.0f, pos.z + 0.0f };
	Vector3 p1 = { pos.x + 0.0f, pos.y + 0.0f, pos.z + 1.0f };
	Vector3 p2 = { pos.x + 0.0f, pos.y + 1.0f, pos.z + 0.0f };
	Vector3 p3 = { pos.x + 0.0f, pos.y + 1.0f, pos.z + 1.0f };
	Vector3 p4 = { pos.x + 1.0f, pos.y + 0.0f, pos.z + 0.0f };
	Vector3 p5 = { pos.x + 1.0f, pos.y + 0.0f, pos.z + 1.0f };
	Vector3 p6 = { pos.x + 1.0f, pos.y + 1.0f, pos.z + 0.0f };
	Vector3 p7 = { pos.x + 1.0f, pos.y + 1.0f, pos.z + 1.0f };

	drawFace( p5, p4, p6, p7, XPCOL );	// +X / right face
	drawFace( p0, p1, p3, p2, XNCOL );	// -X / left face

	drawFace( p3, p7, p6, p2, YPCOL );	// +Y / top face
	drawFace( p5, p1, p0, p4, YNCOL ); // -Y / bottom face

	drawFace( p1, p5, p7, p3, ZPCOL );	// +Z / front face
	drawFace( p4, p0, p2, p6, ZNCOL );	// -Z / back face
}

void	drawFace2( Vector3 pos, int dir )
{
	Vector3 v1, v2, v3, v4;
	Color color;

	if( dir == XPDIR ){
		Vector3 p1 = { pos.x + 1.0f, pos.y + 0.0f, pos.z + 1.0f };
		Vector3 p2 = { pos.x + 1.0f, pos.y + 0.0f, pos.z + 0.0f };
		Vector3 p3 = { pos.x + 1.0f, pos.y + 1.0f, pos.z + 0.0f };
		Vector3 p4 = { pos.x + 1.0f, pos.y + 1.0f, pos.z + 1.0f };
		color = XPCOL;
	}
	elif( dir == XNDIR ){
		v1 = ( Vector3 ){ pos.x + 0.0f, pos.y + 0.0f, pos.z + 0.0f };
		v2 = ( Vector3 ){ pos.x + 0.0f, pos.y + 0.0f, pos.z + 1.0f };
		v3 = ( Vector3 ){ pos.x + 0.0f, pos.y + 1.0f, pos.z + 1.0f };
		v4 = ( Vector3 ){ pos.x + 0.0f, pos.y + 1.0f, pos.z + 0.0f };
		color = XNCOL;
	}
	elif( dir == YPDIR ){
		v1 = ( Vector3 ){ pos.x + 0.0f, pos.y + 1.0f, pos.z + 1.0f };
		v2 = ( Vector3 ){ pos.x + 1.0f, pos.y + 1.0f, pos.z + 1.0f };
		v3 = ( Vector3 ){ pos.x + 1.0f, pos.y + 1.0f, pos.z + 0.0f };
		v4 = ( Vector3 ){ pos.x + 0.0f, pos.y + 1.0f, pos.z + 0.0f };
		color = YPCOL;
	}
	elif( dir == YNDIR ){
		v1 = ( Vector3 ){ pos.x + 1.0f, pos.y + 0.0f, pos.z + 1.0f };
		v2 = ( Vector3 ){ pos.x + 0.0f, pos.y + 0.0f, pos.z + 1.0f };
		v3 = ( Vector3 ){ pos.x + 0.0f, pos.y + 0.0f, pos.z + 0.0f };
		v4 = ( Vector3 ){ pos.x + 1.0f, pos.y + 0.0f, pos.z + 0.0f };
		color = YNCOL;
	}
	elif( dir == ZPDIR ){
		v1 = ( Vector3 ){ pos.x + 0.0f, pos.y + 0.0f, pos.z + 1.0f };
		v2 = ( Vector3 ){ pos.x + 1.0f, pos.y + 0.0f, pos.z + 1.0f };
		v3 = ( Vector3 ){ pos.x + 1.0f, pos.y + 1.0f, pos.z + 1.0f };
		v4 = ( Vector3 ){ pos.x + 0.0f, pos.y + 1.0f, pos.z + 1.0f };
		color = ZPCOL;
	}
	elif( dir == ZNDIR ){
		v1 = ( Vector3 ){ pos.x + 1.0f, pos.y + 0.0f, pos.z + 0.0f };
		v2 = ( Vector3 ){ pos.x + 0.0f, pos.y + 0.0f, pos.z + 0.0f };
		v3 = ( Vector3 ){ pos.x + 0.0f, pos.y + 1.0f, pos.z + 0.0f };
		v4 = ( Vector3 ){ pos.x + 1.0f, pos.y + 1.0f, pos.z + 0.0f };
		color = ZNCOL;
	}
	else{
		fprintf( stderr, "Invalid direction: %d\n", dir );
		return;
	}

	DrawTriangle3D( v1, v2, v3, color );
	DrawTriangle3D( v1, v3, v4, color );

	if( DEBUG ){
		DrawLine3D( v1, v2, BLACK );
		DrawLine3D( v1, v3, BLACK );
		DrawLine3D( v1, v4, BLACK );
		DrawLine3D( v2, v3, BLACK );
		DrawLine3D( v3, v4, BLACK );
	}
}

void	drawVoxel2( bool voxels[S][S][S], Vector3 pos )
{
	if ( !hasVoxel( voxels, pos.x, pos.y, pos.z )){ return; }
	if ( !hasVoxel( voxels, pos.x + 1, pos.y, pos.z )){ drawFace2( pos, XPDIR ); }
	if ( !hasVoxel( voxels, pos.x - 1, pos.y, pos.z )){ drawFace2( pos, XNDIR ); }
	if ( !hasVoxel( voxels, pos.x, pos.y + 1, pos.z )){ drawFace2( pos, YPDIR ); }
	if ( !hasVoxel( voxels, pos.x, pos.y - 1, pos.z )){ drawFace2( pos, YNDIR ); }
	if ( !hasVoxel( voxels, pos.x, pos.y, pos.z + 1 )){ drawFace2( pos, ZPDIR ); }
	if ( !hasVoxel( voxels, pos.x, pos.y, pos.z - 1 )){ drawFace2( pos, ZNDIR ); }

	//drawFace2( pos, XPDIR );
	//drawFace2( pos, XNDIR );
	//drawFace2( pos, YPDIR );
	//drawFace2( pos, YNDIR );
	//drawFace2( pos, ZPDIR );
	//drawFace2( pos, ZNDIR );

}

void	drawGeometry( Camera *camera, bool voxels[S][S][S] )
{
	BeginMode3D( *camera );

	for( int x = 0; x < S; x++ )
		for( int y = 0; y < S; y++ )
			for( int z = 0; z < S; z++ )
				drawVoxel2(voxels, ( Vector3 ){ x, y, z } );

	EndMode3D();
}

void	drawInfoBox( Camera *camera )
{
	int box_x = 5;
	int box_y = 5;
	int	box_width = 380;
	int	box_height = ( 3 * FONT_SIZE ) + 5;

	DrawRectangle		( box_x, box_y, box_width, box_height, Fade( GREEN, 0.5f ));
	DrawRectangleLines	( box_x, box_y, box_width, box_height, VIOLET );

	DrawText( TextFormat( "- Position:(%06.3f, %06.3f, %06.3f)",	camera->position.x,	camera->position.y,	camera->position.z ),	15, 15, FONT_SIZE, BLACK );
	DrawText( TextFormat( "- Target:(%06.3f, %06.3f, %06.3f)",		camera->target.x,	camera->target.y,	camera->target.z ),		15, 40, FONT_SIZE, BLACK );
}
