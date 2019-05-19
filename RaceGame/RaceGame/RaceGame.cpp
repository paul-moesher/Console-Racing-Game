#include "pch.h"
#include <iostream>
#include <string>
#include "olcConsoleGameEngine.h"

using namespace std;

class ConsoleRacingGame : public olcConsoleGameEngine
{
public:
	ConsoleRacingGame()
	{
		m_sAppName = L"Console Racing Game";
	}

private:
	float fCarPos = 0.0f;
	float fDistance = 0.0f;
	float fSpeed = 0.0f;
	float fCurvature = 0.0f;
	float fTrackCurvature = 0.0f;
	float fPlayerCurvature = 0.0f;
	float fTrackDistance = 0.0f;
	float fCurrentLapTime = 0.0f;
	vector<pair<float, float>> vecTrack;
	list<float> listLapTimes;

protected:
	virtual bool OnUserCreate()
	{
		vecTrack.push_back(make_pair(0.0f, 10.0f));
		vecTrack.push_back(make_pair(0.0f, 200.0f));
		vecTrack.push_back(make_pair(1.0f, 200.0f));
		vecTrack.push_back(make_pair(0.0f, 400.0f));
		vecTrack.push_back(make_pair(-1.0f, 100.0f));
		vecTrack.push_back(make_pair(0.0f, 200.0f));
		vecTrack.push_back(make_pair(-1.0f, 200.0f));
		vecTrack.push_back(make_pair(0.2f, 500.0f));
		vecTrack.push_back(make_pair(0.0f, 200.0f));

		for (auto t : vecTrack)
			fTrackDistance += t.second;


		listLapTimes = { 0,0,0,0,0 };

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		if (m_keys[VK_UP].bHeld)
			fSpeed += 2.0f * fElapsedTime;
		else
			fSpeed -= 1.0f * fElapsedTime;

		int nCarDirection = 0;

		if (m_keys[VK_LEFT].bHeld) 
		{
			fPlayerCurvature -= 0.7f * fElapsedTime;
			nCarDirection = -1;
		}

		if (m_keys[VK_RIGHT].bHeld)
		{
			fPlayerCurvature += 0.7f * fElapsedTime;
			nCarDirection = +1;
		}

		if (fabs(fPlayerCurvature - fTrackCurvature) >= 0.8f)
			fSpeed -= 5.0f * fElapsedTime;

		if (fSpeed < 0.0f) fSpeed = 0.0f;
		if (fSpeed > 1.0f) fSpeed = 1.0f;

		fDistance += (70.0f * fSpeed) * fElapsedTime;



		float fOffset = 0;
		int nTrackSection = 0;
		
		fCurrentLapTime += fElapsedTime;
		if (fDistance >= fTrackDistance)
		{
			fDistance -= fTrackDistance;
			listLapTimes.push_front(fCurrentLapTime);
			listLapTimes.pop_back();
			fCurrentLapTime = 0.0f;
		}
		while (nTrackSection < vecTrack.size() && fOffset <= fDistance)
		{
			fOffset += vecTrack[nTrackSection].second;
			nTrackSection++;
		}

		float fTargetCurvature = vecTrack[nTrackSection - 1].first;

		float fTrackCurveDiff = (fTargetCurvature - fCurvature) * fElapsedTime * fSpeed;
		fCurvature += fTrackCurveDiff;

		fTrackCurvature += (fCurvature)* fElapsedTime * fSpeed;

		for (int y = 0; y < ScreenHeight() / 2; y++)
			for (int x = 0; x < ScreenWidth(); x++)
				Draw(x, y, y < ScreenHeight() / 4 ? PIXEL_HALF : PIXEL_SOLID, FG_DARK_BLUE);

		for (int x = 0; x < ScreenWidth(); x++)
		{
			int nHillHeigth = (int)(fabs(sinf(x * 0.01 + fTrackCurvature)*16.0f));
			for (int y = (ScreenHeight() / 2) - nHillHeigth; y < ScreenHeight() / 2; y++)
				Draw(x, y, PIXEL_SOLID, FG_DARK_YELLOW);
		}

		for (int y = 0; y < ScreenHeight() / 2; y++) 
		{

			for (int x = 0; x < ScreenWidth(); x++)
			{
				float fPerspective = (float)y / (ScreenHeight() / 2.0f);

				float fMiddlePoint = 0.5f + fCurvature * powf((1.0f - fPerspective), 3);

				float fRoadWidth = 0.1f + fPerspective * 0.8f;
				float fClipWidth = fRoadWidth * 0.15f;

				fRoadWidth *= 0.5f;

				int nLeftGrass = (fMiddlePoint - fRoadWidth - fClipWidth) * ScreenWidth();
				int nLeftClip = (fMiddlePoint - fRoadWidth) * ScreenWidth();
				int nRightClip = (fMiddlePoint + fRoadWidth) * ScreenWidth();
				int nRightGrass = (fMiddlePoint + fRoadWidth + fClipWidth) * ScreenWidth();

				int nRow = ScreenHeight() / 2 + y;

				int nGrassColor = sinf(20.0f * powf(1.0f - fPerspective, 3) + fDistance * 0.1f) > 0.0f ? FG_GREEN : FG_DARK_GREEN;
				int nClipColor = sinf(80.0f * powf(1.0f - fPerspective, 3) + fDistance * 0.1f) > 0.0f ? FG_RED : FG_WHITE;
				

				int nRoadColor = (nTrackSection - 1) == 0 ? FG_WHITE : FG_GREY;


				if (x >= 0 && x < nLeftGrass)
					Draw(x, nRow, PIXEL_SOLID, nGrassColor);
				if (x >= nLeftGrass && x < nLeftClip)
					Draw(x, nRow, PIXEL_SOLID, nClipColor);
				if (x >= nLeftClip && x < nRightClip)
					Draw(x, nRow, PIXEL_SOLID, nRoadColor);
				if (x >= nRightClip && x < nRightGrass)
					Draw(x, nRow, PIXEL_SOLID, nClipColor);
				if (x >= nRightGrass && x < ScreenWidth())
					Draw(x, nRow, PIXEL_SOLID, nGrassColor);

			}
		}

		fCarPos = fPlayerCurvature - fTrackCurvature;
		int nCarPos = ScreenWidth() / 2 + ((int)(ScreenWidth()* fCarPos) / 2.0f) - 7;

		switch (nCarDirection)
		{
		case 0:
			DrawStringAlpha(nCarPos, 80, L"   ||####||   ");
			DrawStringAlpha(nCarPos, 81, L"      ##   ");
			DrawStringAlpha(nCarPos, 82, L"     ####   ");
			DrawStringAlpha(nCarPos, 83, L"     ####   ");
			DrawStringAlpha(nCarPos, 84, L"|||  ####  |||   ");
			DrawStringAlpha(nCarPos, 85, L"|||########|||   ");
			DrawStringAlpha(nCarPos, 86, L"|||  ####  |||   ");
			break;

		case +1:
			DrawStringAlpha(nCarPos, 80, L"     ||####||");
			DrawStringAlpha(nCarPos, 81, L"        ##   ");
			DrawStringAlpha(nCarPos, 82, L"       ####   ");
			DrawStringAlpha(nCarPos, 83, L"       ####   ");
			DrawStringAlpha(nCarPos, 84, L"    |||####|||   ");
			DrawStringAlpha(nCarPos, 85, L" |||########|||   ");
			DrawStringAlpha(nCarPos, 86, L" |||####|||   ");
			break;

		case -1:
			DrawStringAlpha(nCarPos, 80, L"||####||     ");
			DrawStringAlpha(nCarPos, 81, L"   ##        ");
			DrawStringAlpha(nCarPos, 82, L"   ####       ");
			DrawStringAlpha(nCarPos, 83, L"   ####       ");
			DrawStringAlpha(nCarPos, 84, L"    |||#### |||   ");
			DrawStringAlpha(nCarPos, 85, L"  |||########|||    ");
			DrawStringAlpha(nCarPos, 86, L" |||  ####|||    ");
			break;
		}


		DrawString(0, 0, L"Distance: " + to_wstring(fDistance));
		DrawString(0, 1, L"Target Curvature: " + to_wstring(fCurvature));
		DrawString(0, 2, L"Player Curvature: " + to_wstring(fPlayerCurvature));
		DrawString(0, 3, L"Player Speed: " + to_wstring(fSpeed));
		DrawString(0, 4, L"Track Curvature: " + to_wstring(fTrackCurvature));

		auto disp_time = [](float t)
		{
			int nMinutes = t / 60.0f;
			int nSeconds = t - (nMinutes * 60.0f);
			int nMilliSeconds = (t - (nSeconds) * 1000.0f);
			return  to_wstring(nMinutes) + L"." + to_wstring(nSeconds) + L":" + to_wstring(nMilliSeconds);
		};

		DrawString(10, 8, disp_time(fCurrentLapTime));

		int j = 10;
		for (auto l : listLapTimes)
		{
			DrawString(10, j, disp_time(l));
			j++;
		}

		return true;
	}
};

int main()
{
	ConsoleRacingGame game;
	game.ConstructConsole(160, 100, 8, 8);
	game.Start();
	return 0;
}

