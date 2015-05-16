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
    
    BallSprite* _movingBall;
    bool _movedBall;
    
    bool _touchable;
    int _maxRemovedNo;
    int _chainNumber;
    std::vector<std::map<BallSprite::BallType, int>> _removeNumbers;
    
    void initBackground();
    void initBalls();
    BallSprite* newBalls(BallSprite::PositionIndex positionIndex, bool visible);
    
    BallSprite* getTouchBall(cocos2d::Point touchPos,
                             BallSprite::PositionIndex withoutPosIndex = BallSprite::PositionIndex());
    
    //ボール操作終了処理
    void movedBall();
    
    /*ボールの消去とアニメーションに関する処理*/
    void checksLinedBalls();
    bool existsLinedBalls();
    cocos2d::Map<int, BallSprite*> getAllBalls();
    bool isSameBallType(BallSprite::PositionIndex current, Direction direction);
    void initBallParms();
    void checkedBall(BallSprite::PositionIndex current, Direction direction);
    void removeAndGenerateBalls();
    void generateBalls(int xlineNum, int fallCount);
    void animationBalls();
    
public:
    GameLayer();
    virtual bool init();
    CREATE_FUNC(GameLayer);
    static cocos2d::Scene* createScene();

    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* unused_event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* unused_event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* unused_event);
    virtual void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* unused_event);
};


#endif /* defined(__PuzzleAndGames__GameLayer__) */
