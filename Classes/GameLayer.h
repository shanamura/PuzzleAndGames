//
//  GameLayer.h
//  PuzzleAndGames
//
//  Created by hashi on 2015/05/15.
//
//

#ifndef __PuzzleAndGames__GameLayer__
#define __PuzzleAndGames__GameLayer__

#include "cocos2d.h"
#include "BallSprite.h"
#include <random>

class GameLayer : public cocos2d::Layer
{
protected:
    //ボールチェックの方向
    enum Direction
    {
        x,
        y,
    };
    
    //ZOrder
    enum ZOrder
    {
        BgForCharacter = 0,
        BgForPuzzle,
        Enemy,
        EnemyHp,
        Char,
        CharHp,
        Ball,
        Level,
        Result,
    };
    
    std::default_random_engine _engine;
    std::discrete_distribution<int> _distForBall;
    
    void initBackground();
    void initBalls();
    BallSprite* newBalls(BallSprite::PositionIndex positionIndex);
    
public:
    GameLayer();
    virtual bool init();
    CREATE_FUNC(GameLayer);
    static cocos2d::Scene* createScene();
};


#endif /* defined(__PuzzleAndGames__GameLayer__) */
