#pragma once

#ifndef DLLAPI
#define DLLAPI extern "C" __declspec(dllexport)
#endif

typedef struct _GAMESTATE{
	int		ShotNum;		// ���݂̃V���b�g��
							// ShotNum �� n �̏ꍇ�A���ɍs���V���b�g�� n+1 ���ڂɂȂ�

	int		CurEnd;			// ���݂̃G���h��
	int		LastEnd;		// �ŏI�G���h��
	int		Score[10];		// ��1�G���h�����10�G���h�܂ł̃X�R�A
	bool	WhiteToMove;	// ��Ԃ̏��
							// WhiteToMove �� 0 �̏ꍇ���̃V���b�g���s���̂͐��AWhiteToMove �� 1 �̏ꍇ���̃V���b�g���s���̂͌��ƂȂ�

	float	body[16][2];	// body[n] �� n ���ڂ̃X�g�[���̈ʒu���W��\��
							// body[n][0] �� n ���ڂ̃X�g�[���� x ���W�Abody[n][1] �� n ���ڂ̃X�g�[���� y ���W��\��

	/* �R���X�g���N�^�i�V�~�����[�^ver3�j */
	_GAMESTATE() : ShotNum(0), CurEnd(0), LastEnd(10), Score(), WhiteToMove(0), body() {};
	_GAMESTATE(int _LastEnd) : ShotNum(0), CurEnd(), LastEnd(_LastEnd), Score(), WhiteToMove(0), body() {};

} GAMESTATE, *PGAMESTATE;

// �V���b�g
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

// �s�v�ȏꍇ�͈ȉ��R�����g�ɂ��Ă�������
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
