/******************************************************************************************
*	Chili DirectX Framework Version 16.10.01											  *
*	Game.cpp																			  *
*	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include "Box.h"
#include <algorithm>
#include <sstream>
#include <typeinfo>
#include <functional>
#include <iterator>

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	world( { 0.0f,-0.5f } ),
	pepe( gfx )
{
	pepe.effect.vs.cam.SetPos( { 0.0,0.0f } );
	pepe.effect.vs.cam.SetZoom( 1.0f / boundarySize );

	std::generate_n( std::back_inserter( boxPtrs ),nBoxes,[this]() {
		return Box::Spawn( boxSize,bounds,world,rng );
	} );

	world.SetContactListener( &listener );
}

void Game::Go()
{
	gfx.BeginFrame();
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	const float dt = ft.Mark();
	world.Step( dt,8,3 );
	for (auto it_map = listener.contactPatternMap.begin(); it_map != listener.contactPatternMap.end();)
	{
		b2Contact* contact = it_map->first;
		b2Body* contactBodies[2] = {
			contact->GetFixtureA()->GetBody(),
			contact->GetFixtureB()->GetBody() };

		Color contactBodyColors[2] = { 
			reinterpret_cast<Box*>(contactBodies[0]->GetUserData())->GetColorTrait().GetColor(),
			reinterpret_cast<Box*>(contactBodies[1]->GetUserData())->GetColorTrait().GetColor() };

		b2Body* muteBody = nullptr;

		if (it_map->second == ContactListener::Split)
		{
			enum Tiles
			{
				TopLeft,
				TopRight,
				BottomLeft,
				BottomRight
			};
			std::vector<std::pair<Tiles, Vec2>> tiles =
			{
				{ TopLeft,		{ -0.5f,	-0.5f } },
				{ TopRight,		{  0.5f,	-0.5f } },
				{ BottomLeft,	{ -0.5f,	 0.5f } },
				{ BottomRight,	{  0.5f,	 0.5f } }
			};

			for (int i = 0; i < 4; ++i)
			{
				muteBody = contactBodyColors[0] == Colors::White ? contactBodies[1] : contactBodies[0];
				if (reinterpret_cast<Box*>(muteBody->GetUserData())->GetSize() > boxSize / 2)
				{
					Vec2 pos = Vec2(muteBody->GetPosition()) + tiles[i].second;

					Color color = reinterpret_cast<Box*>(muteBody->GetUserData())->GetColorTrait().GetColor();
					boxPtrs.emplace_back(
						Box::Spawn(
							pos, color, boxSize / 2,
							muteBody->GetAngle(), Vec2(muteBody->GetLinearVelocity()), muteBody->GetAngularVelocity(),
							bounds, world, rng
						)
					);
					it_map->second = ContactListener::Remove;
				}
			}
			if (it_map->second == ContactListener::Split)
				it_map = listener.contactPatternMap.erase(it_map);
			continue;
		}

		if (it_map->second == ContactListener::Wololo)
		{
			muteBody = contactBodyColors[0] == Colors::Blue ? contactBodies[1] : contactBodies[0];
			reinterpret_cast<Box*>(muteBody->GetUserData())->SetColor(Colors::Blue);
			it_map = listener.contactPatternMap.erase(it_map);
			continue;
		}

		if (it_map->second == ContactListener::Remove)
		{
			if (muteBody == nullptr)
				muteBody = contactBodyColors[0] == Colors::Red ? contactBodies[1] : contactBodies[0];
			for (auto it_vec = boxPtrs.begin(); it_vec != boxPtrs.end();)
			{
				if (it_vec->get() == muteBody->GetUserData())
				{
					it_vec = boxPtrs.erase(it_vec);
				}
				else
					++it_vec;
			}
			it_map = listener.contactPatternMap.erase(it_map);
		}
		else
			++it_map;
	}
}

void Game::ComposeFrame()
{
	for( const auto& p : boxPtrs )
	{
		p->Draw( pepe );
	}
}