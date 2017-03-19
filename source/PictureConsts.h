// Képi inputok és konstansok

#ifndef __PICTURE_CONSTS_H__
#define __PICTURE_CONSTS_H__

// Irányítási konstansok
#define ESC_KEY         27   // Kilépés gomb
#define WAITKEY_TIME     1   // Waitkey idõ konstans (ms)

// Videó inputok
#define VIDEO_1          "TEST\\videos\\chess1_9fps.wmv"

// Kamera input
#define CAMERA_ID  1

// A sakklépés figyelõ mintavételi ideje (ms)
#define SAMPLING_TIME_MS   200

// FPS:
#define FPS  9   // Frames per sec az input videóban

#define TIME_BW_FRAMES_MS (int) (1000 / FPS)  // Frame-ek között eltelt idõ ms-ben az input videóban


// GUI
#define STARTING_SCREEN   "source\\pictures\\starting_screen.jpg"
#define GUI_BASE "source\\pictures\\gui_base.jpg"

#define black_bishop_on_black "source\\pictures\\pieces\\black_bishop_on_black.png"
#define black_bishop_on_white "source\\pictures\\pieces\\black_bishop_on_white.png"
#define black_king_on_black "source\\pictures\\pieces\\black_king_on_black.png"
#define black_king_on_white "source\\pictures\\pieces\\black_king_on_white.png"
#define black_knight_on_black "source\\pictures\\pieces\\black_knight_on_black.png"
#define black_knight_on_white "source\\pictures\\pieces\\black_knight_on_white.png"
#define black_pawn_on_black "source\\pictures\\pieces\\black_pawn_on_black.png"
#define black_pawn_on_white "source\\pictures\\pieces\\black_pawn_on_white.png"
#define black_queen_on_black "source\\pictures\\pieces\\black_queen_on_black.png"
#define black_queen_on_white "source\\pictures\\pieces\\black_queen_on_white.png"
#define black_rook_on_black "source\\pictures\\pieces\\black_rook_on_black.png"
#define black_rook_on_white "source\\pictures\\pieces\\black_rook_on_white.png"
#define white_bishop_on_black "source\\pictures\\pieces\\white_bishop_on_black.png"
#define white_bishop_on_white "source\\pictures\\pieces\\white_bishop_on_white.png"
#define white_king_on_black "source\\pictures\\pieces\\white_king_on_black.png"
#define white_king_on_white "source\\pictures\\pieces\\white_king_on_white.png"
#define white_knight_on_black "source\\pictures\\pieces\\white_knight_on_black.png"
#define white_knight_on_white "source\\pictures\\pieces\\white_knight_on_white.png"
#define white_pawn_on_black "source\\pictures\\pieces\\white_pawn_on_black.png"
#define white_pawn_on_white "source\\pictures\\pieces\\white_pawn_on_white.png"
#define white_queen_on_black "source\\pictures\\pieces\\white_queen_on_black.png"
#define white_queen_on_white "source\\pictures\\pieces\\white_queen_on_white.png"
#define white_rook_on_black "source\\pictures\\pieces\\white_rook_on_black.png"
#define white_rook_on_white "source\\pictures\\pieces\\white_rook_on_white.png"

#endif __PICTURE_CONSTS_H__