//
//  GameLayer.cpp
//  PuzzleAndGames
//
//  Created by hashi on 2015/05/15.
//
//

#include "GameLayer.h"

#define BALL_NUM_X 6
#define BALL_NUM_Y 5

USING_NS_CC;

GameLayer::GameLayer()
{
    std::random_device device;
    _engine = std::default_random_engine(device());
    _distForBall = std::discrete_distribution<int>{20, 20, 20, 20, 20, 10};
}

Scene* GameLayer::createScene()
{
    auto scene = Scene::create();
    auto layer = GameLayer::create();
    
    scene->addChild(layer);
    
    return scene;
}

//初期化
bool GameLayer::init()
{
    if(!Layer::init())
    {
        return false;
    }
    
    initBackground();
    initBalls();
    
    return true;
}

void GameLayer::initBackground()
{
    auto bgForPuzzle = Sprite::create("Background2.png");
    bgForPuzzle->setAnchorPoint(Point::ZERO);
    bgForPuzzle->setPosition(Point::ZERO);
    
    addChild(bgForPuzzle, BgForPuzzle);
}

void GameLayer::initBalls()
{
    for(int x = 1; x <= BALL_NUM_X; x++)
    {
        for(int y = 1; y <= BALL_NUM_Y; y++)
        {
            newBalls(BallSprite::PositionIndex(x, y));
        }
    }
}

BallSprite* GameLayer::newBalls(BallSprite::PositionIndex positionIndex)
{
    int currentTag = BallSprite::generateTag(positionIndex);
    
    int ballType;
    while(true)
    {
        ballType = _distForBall(_engine);
        
        //同じ色のボールが３つつながらないようにチェックしつつボールを配置
        auto ballX1Tag = currentTag - 10;
        auto ballX2Tag = currentTag - 20;
        auto ballX1 = (BallSprite*)(getChildByTag(ballX1Tag));
        auto ballX2 = (BallSprite*)(getChildByTag(ballX2Tag));
        
        //横方向にチェック
        if(!(ballX1 && ballType == (int)ballX1->getBallType()) ||
           !(ballX2 && ballType == (int)ballX2->getBallType()))
        {
            auto ballY1Tag = currentTag - 1;
            auto ballY2Tag = currentTag - 2;
            auto ballY1 = (BallSprite*)(getChildByTag(ballY1Tag));
            auto ballY2 = (BallSprite*)(getChildByTag(ballY2Tag));
            
            //縦方向にチェック
            if(!(ballY1 && ballType == (int)ballY1->getBallType()) ||
               !(ballY2 && ballType == (int)ballY2->getBallType()))
            {
                break;
            }
        }
    }
    
    //ボールの作成
    auto ball = BallSprite::create((BallSprite::BallType)ballType, true);
    ball->setPositionIndexAndChangePosition(positionIndex);
    addChild(ball, Ball);
    
    return ball;
}


