#pragma once

#ifndef DLLAPI
#define DLLAPI extern "C" __declspec(dllexport)
#endif

typedef struct _GAMESTATE{
	int		ShotNum;		// 現在のショット数
							// ShotNum が n の場合、次に行うショットが n+1 投目になる

	int		CurEnd;			// 現在のエンド数
	int		LastEnd;		// 最終エンド数
	int		Score[10];		// 第1エンドから第10エンドまでのスコア
	bool	WhiteToMove;	// 手番の情報
							// WhiteToMove が 0 の場合次のショットを行うのは先手、WhiteToMove が 1 の場合次のショットを行うのは後手となる

	float	body[16][2];	// body[n] は n 投目のストーンの位置座標を表す
							// body[n][0] は n 投目のストーンの x 座標、body[n][1] は n 投目のストーンの y 座標を表す

	/* コンストラクタ（シミュレータver3） */
	_GAMESTATE() : ShotNum(0), CurEnd(0), LastEnd(10), Score(), WhiteToMove(0), body() {};
	_GAMESTATE(int _LastEnd) : ShotNum(0), CurEnd(), LastEnd(_LastEnd), Score(), WhiteToMove(0), body() {};

} GAMESTATE, *PGAMESTATE;

// ショット
typedef struct _ShotPos{
	float x;
	float y;
	bool angle;

	_ShotPos() : x(0), y(0), angle(0) {};
	_ShotPos(float _x, float _y, bool _angle) : x(_x), y(_y), angle(_angle) {};
} SHOTPOS, PSHOTPOS;

typedef struct _ShotVec{
	float x;
	float y;
	bool angle;

	_ShotVec() : x(0), y(0), angle(0) {};
	_ShotVec(float _x, float _y, bool _angle) : x(_x), y(_y), angle(_angle) {};
} SHOTVEC, PSHOTVEC;

// 不要な場合は以下コメントにしてください
//DLLAPI int Simulation( GAMESTATE *pGameState, SHOTVEC Shot, float Rand, SHOTVEC *lpResShot, int LoopCount );
//DLLAPI int SimulationEx( GAMESTATE *pGameState, SHOTVEC Shot, float RandX, float RandY, SHOTVEC *lpResShot, float *pBody,/ size_t ResBodySize, int LoopCount );
//DLLAPI int CreateShot( SHOTPOS ShotPos, SHOTVEC *lpResShotVec );
//DLLAPI int CreateHitShot( SHOTPOS Shot, float Power, SHOTVEC *lpResShot );
//DLLAPI int GetScore( GAMESTATE *pGameState );

typedef int (*SIMULATION_FUNC)( GAMESTATE *pGameState, SHOTVEC Shot, float Rand, SHOTVEC *lpResShot, int LoopCount );
typedef int (*SIMULATIONEX_FUNC)( GAMESTATE *pGameState, SHOTVEC Shot, float RandX, float RandY, SHOTVEC *lpResShot, float *pBody, size_t ResBodySize, int LoopCount );
typedef int (*CREATESHOT_FUNC)( SHOTPOS ShotPos, SHOTVEC *lpResShotVec );
typedef int (*CREATEHITSHOT_FUNC)( SHOTPOS Shot, float Power, SHOTVEC *lpResShot );
typedef int (*GETSCORE_FUNC)( GAMESTATE *pGameState );
