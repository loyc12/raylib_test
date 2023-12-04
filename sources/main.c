#include <stdlib.h>
#include "raylib.h"
#include "rcamera.h"

#define SCREEN_WIDTH	2048
#define SCREEN_HEIGHT	1024
#define WINDOW_TITLE	"Voxel Test"
#define FONT_SIZE		20

#define ROT_FACTOR		0.1f
#define MOV_FACTOR		0.2f


#define FNP		(void**)&
void nullFree	(void **ptrloc){ free( *ptrloc ); *ptrloc = 0; }

//#define FNA			(void***)&
//void nullFreeArray	(void ***aploc, int size){ for (int i = 0; i < size; i++){ nullFree( aploc[i] ); }}

Camera	*initPlayerCam();

void	refreshView( Camera *camera );
void	drawGeometry( Camera *camera );
void	drawInfoBox( Camera *camera);

int main(void)
{
	// Initialize the camera
	Camera *camera = initPlayerCam();

	// Main game loop
	while( !WindowShouldClose() )	// Detect window close button or ESC key
	{
		refreshView(camera);

		// Draw phase
		BeginDrawing();

			ClearBackground( DARKGRAY );

			//drawGeometry( camera, heights, positions, colours );
			drawGeometry( camera );

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

void drawFace( Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Color color )
{
	DrawTriangle3D( v1, v2, v3, color );
	DrawTriangle3D( v1, v3, v4, color );

	DrawLine3D( v1, v2, BLACK );
	DrawLine3D( v1, v3, BLACK );
	DrawLine3D( v1, v4, BLACK );
	DrawLine3D( v2, v3, BLACK );
	DrawLine3D( v3, v4, BLACK );
}

void drawVoxel( Vector3 pos)
{
	Vector3 p0 = { pos.x + 0.0f, pos.y + 0.0f, pos.z + 0.0f };
	Vector3 p1 = { pos.x + 0.0f, pos.y + 0.0f, pos.z + 1.0f };
	Vector3 p2 = { pos.x + 0.0f, pos.y + 1.0f, pos.z + 0.0f };
	Vector3 p3 = { pos.x + 0.0f, pos.y + 1.0f, pos.z + 1.0f };
	Vector3 p4 = { pos.x + 1.0f, pos.y + 0.0f, pos.z + 0.0f };
	Vector3 p5 = { pos.x + 1.0f, pos.y + 0.0f, pos.z + 1.0f };
	Vector3 p6 = { pos.x + 1.0f, pos.y + 1.0f, pos.z + 0.0f };
	Vector3 p7 = { pos.x + 1.0f, pos.y + 1.0f, pos.z + 1.0f };

	drawFace( p0, p1, p3, p2, RED );	// -X / left face
	drawFace( p5, p4, p6, p7, ORANGE );	// +X / right face

	drawFace( p5, p1, p0, p4, VIOLET ); // -Y / bottom face
	drawFace( p3, p7, p6, p2, BLUE );	// +Y / top face

	drawFace( p4, p0, p2, p6, GREEN );	// -Z / back face
	drawFace( p1, p5, p7, p3, YELLOW );	// +Z / front face
}


void	drawGeometry( Camera *camera )
{
	BeginMode3D( *camera );

	int size = 32;

	for( int x = 0; x < size; x++ )
		for( int y = 0; y < size; y++ )
			for( int z = 0; z < size; z++ )
			{
				if( x % 2 || y % 2 || z % 2 || ( x + y + z ) % 4 )
					continue;
				drawVoxel( ( Vector3 ){ x, y, z } );
			}

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
