//
//  BallSprite.cpp
//  PuzzleAndGames
//
//  Created by hashi on 2015/05/14.
//
//

#include "BallSprite.h"

USING_NS_CC;

BallSprite::BallSprite()
: _removeNo(0)
, _checkedX(false)
, _checkedY(false)
, _fallCount(0)
, _positionIndex(0,0)
{
}

//インスタンスの生成
BallSprite* BallSprite::create(BallType type, bool visible)
{
    BallSprite *pRet = new BallSprite();
    if(pRet && pRet->init(type, visible))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

//初期化
bool BallSprite::init(BallType type, bool visible)
{
    if(!Sprite::initWithFile(getBallImageFilePath(type)))
    {
        return false;
    }
    
    _ballType = type;
    setVisible(visible);
    
    return true;
}

void BallSprite::resetParams()
{
    _removeNo = 0;
    _checkedX = false;
    _checkedY = false;
    _fallCount = 0;
}

//ボールの初期位置を変更
void BallSprite::resetPosition()
{
    setPosition(getPositionForPositionIndex(_positionIndex));
}

//位置インデックスを返す
BallSprite::PositionIndex BallSprite::getPositionIndex()
{
    return _positionIndex;
}

//位置インデックスとタグを変更
void BallSprite::setPositionIndex(PositionIndex positionIndex)
{
    _positionIndex = positionIndex;
    
    setTag(generateTag(_positionIndex));
}


void BallSprite::setPositionIndexAndChangePosition(PositionIndex positionIndex)
{
    setPositionIndex(positionIndex);
    
    resetPosition();
}

//ボールの消去と落下アニメーションの呼び出し
void BallSprite::removingAndFallingAnimation(int maxRemovedNo)
{
    removingAnimation(maxRemovedNo);
    
    fallingAnimation(maxRemovedNo);
}

//ボールの消去アニメーション
void BallSprite::removingAnimation(int maxRemovedNo)
{
    if(_removeNo > 0)
    {
        auto delay1 = DelayTime::create(ONE_ACTION_TIME * (_removeNo - 1));
        auto fade = FadeTo::create(ONE_ACTION_TIME, 0);
        auto delay2 = DelayTime::create(ONE_ACTION_TIME * (maxRemovedNo - 1));
        
        auto removeSelf = RemoveSelf::create(false);
        
        runAction(Sequence::create(delay1, fade, delay2, removeSelf, nullptr));
    }
}

//ボールの落下アニメーション
void BallSprite::fallingAnimation(int maxremovedNo)
{
    if(_fallCount > 0)
    {
        setPositionIndex(PositionIndex(_positionIndex.x, _positionIndex.y - _fallCount));
        
        auto delay1 = DelayTime::create(ONE_ACTION_TIME * (maxremovedNo - 1));
        auto show = Show::create();
        auto move = MoveTo::create(ONE_ACTION_TIME, getPositionForPositionIndex(PositionIndex()));
        
        runAction(Sequence::create(delay1, show, move, nullptr));
    }
}

//ボールの取得
std::string BallSprite::getBallImageFilePath(BallType type)
{
    switch (type) {
        case BallType::Blue:
            return "blue.png";
        case BallType::Red:
            return "red.png";
        case BallType::Green:
            return "green.png";
        case BallType::Pink:
            return "pink.png";
        case BallType::Purple:
            return "purple.png";
        case BallType::Yellow:
            return "yellow.png";
    }
}

//位置インデックスからボールの位置を取得
Point BallSprite::getPositionForPositionIndex(PositionIndex positionIndex)
{
    return Point(BALL_SIZE * (positionIndex.x - 0.5) + 1, BALL_SIZE * (positionIndex.y - 0.5) + 1);
}

//タグの所得
int BallSprite::generateTag(PositionIndex positionIndex)
{
    return positionIndex.x * 10 + positionIndex.y;
}