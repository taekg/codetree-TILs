#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<algorithm>
#include<vector>
#include<cstring>

using namespace std;

struct Santa
{
	int Num;
	int y;
	int x;
	int dis;
	int Score;
	bool die;
	int down;

	bool operator<(Santa right) const {
		// 거리에 따라 체크
		if (dis < right.dis)return true;
		if (dis > right.dis)return false;

		// 거리가 같다면 아래에 있는지 체크
		if (y > right.y)return true;
		if (y < right.y)return false;

		// 같은 행이라면 오른쪽에 있는지 체크
		if (x > right.x)return true;
		if (x < right.x)return false;

		return false;
		// 소트결과 
		// 살아있으면서 가장 맨 앞에 있는 산타가 돌진 대상이 된다.
	}
};

bool SeqOperater(Santa left, Santa right) {
	if (left.Num < right.Num)return true;
	if (left.Num > right.Num)return false;
	return false;
}

// 맵의크기
int N;
// 턴의 개수
int M;
// 산타수
int P;
// 루돌프의 힘
int C;
// 산타의 힘
int D;
// 턴 변수
int Turn;
int MAP[50][50];

// 루돌프의 처음 위치
int ruX;
int ruY;

// 산타벡터 선언
vector<Santa>Sv;

// 산타 방향 벡터
int dy[4] = { -1,0,1,0 };
int dx[4] = { 0,1,0,-1 };

// 방향 판단 용 방향벡터
int findy[8] = { -1,-1,-1,0,1,1,1,0 };
int findx[8] = { -1,0,1,1,1,0,-1,-1 };

void input() {
	cin >> N >> M >> P >> C >> D;
	cin >> ruY >> ruX;
	ruY--;
	ruX--;
	for (int i = 1; i <= P; i++)
	{
		int Num, y, x = 0;
		cin >> Num >> y >> x;
		// 산타 벡터 입력받는다.
		Sv.push_back({ Num,y - 1,x - 1,99999,0, false, 0 });
	}
	// 산타 번호 순서대로 정렬 진행
	sort(Sv.begin(), Sv.end(), SeqOperater);
}

void betEffet(int push, int dir) {
	// 일단 상호작용은 산타와 산타 사이에서 발생한다.
	// push는 밀려난 산타 인덱스
	// dir는 밀린방향

	// push번째 산타는 해당 방향으로 1 밀린다.
	Sv[push].y += findy[dir];
	Sv[push].x += findx[dir];

	// 밀렸는데 맵 밖이면 죽여라
	if (Sv[push].y >= N || Sv[push].x >= N || Sv[push].y < 0 || Sv[push].x < 0) {
		Sv[push].die = true;
	}

	// 밀린 위치가 혹시 다른 산타면 그 산타는 또 밀린다.
	// 재귀함수 사용
	for (int i = 0; i < P; i++)
	{
		if (Sv[i].die)continue;
		if (i == push)continue;
		if (Sv[push].y == Sv[i].y && Sv[push].x == Sv[i].x) {
			betEffet(i, dir);
		}
	}
}

void Crash(int whoCrash, int Attack, int dir) {

	// 루돌프가 박았을때
	if (whoCrash == 1) {
		// 기절한다
		// 몇번째 턴에 기절하는지 적는다.
		// 최종적으로 기절이 풀리는 턴을 저장한다.
		Sv[Attack].down = Turn + 2;
		// C만큼의 점수를 얻는다.
		Sv[Attack].Score += C;
		// 방향 그대로 산타가 밀려난다.
		for (int i = 0; i < C; i++)
		{
			Sv[Attack].y += findy[dir];
			Sv[Attack].x += findx[dir];
		}
		// 이동한 위치가 맵 밖이라면 죽는다.
		if (Sv[Attack].y >= N || Sv[Attack].x >= N || Sv[Attack].y < 0 || Sv[Attack].x < 0) {
			Sv[Attack].die = true;
		}
		// 이동한 위치에 다른 산타가 있으면~~~~
		// 상호작용한다.
		for (int i = 0; i < P; i++)
		{
			if (i == Attack)continue;
			if (Sv[i].y == Sv[Attack].y && Sv[i].x == Sv[Attack].x) {
				betEffet(i, dir);
			}
		}
	}
	// 산타가 박았을때
	else {
		// 기절한다
		// 몇번째 턴에 기절하는지 적는다.
		// 최종적으로 기절이 풀리는 턴을 저장한다.
		Sv[Attack].down = Turn + 2;
		// D만큼의 점수를 얻는다.
		Sv[Attack].Score += D;
		// 박은 방향의 반대로 진행한다.
		int reverseDir = (dir + 4) % 8;
		for (int i = 0; i < D - 1; i++)
		{
			Sv[Attack].y += findy[reverseDir];
			Sv[Attack].x += findx[reverseDir];
		}
		// 이동한 위치가 맵 밖이라면 죽는다.
		if (Sv[Attack].y >= N || Sv[Attack].x >= N || Sv[Attack].y < 0 || Sv[Attack].x < 0) {
			Sv[Attack].die = true;
		}
		// 이동한 위치에 다른 산타가 있으면~~~~
		// 상호작용한다.
		for (int i = 0; i < P; i++)
		{
			if (i == Attack)continue;
			if (Sv[i].y == Sv[Attack].y && Sv[i].x == Sv[Attack].x) {
				betEffet(i, reverseDir);
			}
		}
	}
}

void moveDear() {

	int Savedir = 0;
	// 가장 가까운 산타를 향해 움직인다.
	// 현재 루돌프와의 거리를 각 갱신한다.
	// 죽은 산타말고는 기절한 산타한테는 간다.
	for (int i = 0; i < Sv.size(); i++)
	{
		if (Sv[i].die)continue;
		int Dis = (ruY - Sv[i].y) * (ruY - Sv[i].y) + (ruX - Sv[i].x) * (ruX - Sv[i].x);
		Sv[i].dis = Dis;
	}

	// 돌진 대상을 선정하기 위한 sort
	sort(Sv.begin(), Sv.end());

	int targetY = 0;
	int targetX = 0;
	int Saveint = 0;

	// 돌진 대상 정하기
	for (int i = 0; i < P; i++)
	{
		if (Sv[i].die)continue;
		Saveint = i;
		targetY = Sv[i].y;
		targetX = Sv[i].x;
		break;
	}

	// 방향벡터 왼쪽 위부터 0
	// y, x 값중 하나라도 같은 게 있다 => 직진으로 이동
	if (targetY == ruY || targetX == ruX) {
		if (targetX == ruX) {
			if (targetY < ruY) {
				Savedir = 1;
				ruY--;
			}
			else {
				Savedir = 5;
				ruY++;
			}
		}
		else if (targetY == ruY) {
			if (targetX < ruX) {
				Savedir = 7;
				ruX--;
			}
			else {
				Savedir = 3;
				ruX++;
			}
		}
	}
	// 다르다 => 대각선으로 이동
	else {
		// 돌진대상이 1사분면에 있다 (y가 작고 x가 크다)
		if (targetY < ruY && targetX > ruX) {
			ruY--;
			ruX++;
			Savedir = 2;
		}
		// 돌진대상이 2사분면에 있다 (y가 크고 x가 크다)
		else if (targetY > ruY && targetX > ruX) {
			ruY++;
			ruX++;
			Savedir = 4;
		}
		// 돌진대상이 3사분면에 있다 (y가 크고 x가 작다)
		else if (targetY > ruY && targetX < ruX) {
			ruY++;
			ruX--;
			Savedir = 6;
		}
		// 돌진대상이 4사분면에 있다 (y가 작고 x가 작다)
		else if (targetY < ruY && targetX < ruX) {
			ruY--;
			ruX--;
			Savedir = 0;
		}
	}

	// 이동을 마치고 돌진대상에게 충돌했는지 판단
	// 충돌했다면
	if (ruY == targetY && ruX == targetX) {
		// 루돌프가 갖다 박았다면 => 1
		// 산타가 갖다 박았다면 => 0
		// 1, 0의 의미 루돌프가 0번째의 산타를 박았다.
		Crash(1, Saveint, Savedir);
	}
}

void moveSanta() {

	// 일단 산타를 맵에 뿌린다.
	memset(MAP, 0, sizeof(MAP));
	for (int i = 0; i < P; i++)
	{
		// 죽은 산타는 continue;
		if (Sv[i].die)continue;
		MAP[Sv[i].y][Sv[i].x] = 1;
	}

	sort(Sv.begin(), Sv.end(), SeqOperater);
	// 산타수만큼 반복
	// 1번부터 P번까지 반복
	// 그러나 ~ 기절하거나 죽은 산타는 continue~

	for (int i = 0; i < P; i++)
	{
		// 기절해있는 산타는 continue;
		if (Sv[i].down > Turn)continue;
		// 죽은 산타는 continue;
		if (Sv[i].die)continue;

		int Mindis = 21e8;
		// 4방향을 돌면서 루돌프까지의 4방향 각각의 남은 거리를 구한다
		// 가장 작은 남은 거리를 저장한다.
		// 현재 위치에서 거리 계산
		int nowDis = (Sv[i].y - ruY) * (Sv[i].y - ruY) + (Sv[i].x - ruX) * (Sv[i].x - ruX);
		Mindis = nowDis;
		for (int d = 0; d < 4; d++)
		{
			int ny = Sv[i].y + dy[d];
			int nx = Sv[i].x + dx[d];
			if (ny >= N || nx >= N || ny < 0 || nx < 0)continue;
			if (MAP[ny][nx] == 1)continue;
			int resdis = (ruY - ny) * (ruY - ny) + (ruX - nx) * (ruX - nx);
			Mindis = min(Mindis, resdis);
		}
		// 4방향을 돌면서 가장 작은 남은 거리만 고려한다.
		// 하지만 산타가 있거나 맵 밖이면 가지 않는다.
		for (int d = 0; d < 4; d++)
		{
			int ny = Sv[i].y + dy[d];
			int nx = Sv[i].x + dx[d];
			if (ny >= N || nx >= N || ny < 0 || nx < 0)continue;
			// 산타가 있다면 가지마라
			if (MAP[ny][nx] == 1)continue;
			int resdis = (ruY - ny) * (ruY - ny) + (ruX - nx) * (ruX - nx);
			if (Mindis != resdis)continue;

			// 위의 가지치기를 통과했다면 이동하고 break;
			// 하지만 그 위치가 루돌프라면 충돌하고 나서 위치 갱신
			if (ny == ruY && nx == ruX) {

				Crash(0, i, d * 2 + 1);
				// 맵 갱신
				memset(MAP, 0, sizeof(MAP));
				for (int i = 0; i < P; i++)
				{
					if (Sv[i].die)continue;
					MAP[Sv[i].y][Sv[i].x] = 1;
				}
			}
			// 루돌프 안부딫히면 뭐 갱신 안해도 되긴하지
			else {
				MAP[ny][nx] = 1;
				MAP[Sv[i].y][Sv[i].x] = 0;
				Sv[i].y = ny;
				Sv[i].x = nx;
			}
			break;
		}
	}
}

int main() {

	input();
	for (int i = 0; i < M; i++)
	{
		// 루돌프부터 움직이자
		moveDear();
		// 그 다음 산타들 이동
		moveSanta();
		int dieCnt = 0;
		for (int i = 0; i < P; i++)
		{
			if (Sv[i].die) {
				dieCnt++;
				continue;
			}
			Sv[i].Score++;
		}
		// 산타가 다 죽었으면 더 이상 할필요 X
		if (dieCnt == P) {
			break;
		}
		Turn++;
	}
	sort(Sv.begin(), Sv.end(), SeqOperater);
	for (int i = 0; i < P; i++)
	{
		cout << Sv[i].Score << " ";
	}

	return 0;
}