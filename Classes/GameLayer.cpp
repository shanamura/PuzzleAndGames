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

//コンストラクタ
GameLayer::GameLayer()
: _movingBall(nullptr)
, _movedBall(false)
, _touchable(true)
, _maxRemovedNo(0)
, _chainNumber(0)
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
    
    //タッチ操作のイベント取得
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(GameLayer::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(GameLayer::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(GameLayer::onTouchEnded, this);
    touchListener->onTouchCancelled = CC_CALLBACK_2(GameLayer::onTouchCancelled, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
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
            newBalls(BallSprite::PositionIndex(x, y), true);
        }
    }
}

BallSprite* GameLayer::newBalls(BallSprite::PositionIndex positionIndex,
                                bool visible)
{
    int currentTag = BallSprite::generateTag(positionIndex);
    
    int ballType;
    while(true)
    {
        ballType = _distForBall(_engine);
        
        if(!visible)
        {
            break;
        }
        
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
    auto ball = BallSprite::create((BallSprite::BallType)ballType, visible);
    ball->setPositionIndexAndChangePosition(positionIndex);
    addChild(ball, Ball);
    
    return ball;
}

//タップしたボールの取得
BallSprite* GameLayer::getTouchBall(Point touchPos, BallSprite::PositionIndex withoutposIndex)
{
    for(int x = 1; x <= BALL_NUM_X; x++)
    {
        for(int y = 1; y <= BALL_NUM_Y; y++)
        {
            if(x == withoutposIndex.x && y == withoutposIndex.y)
            {
                continue;
            }
            
            int tag = BallSprite::generateTag(BallSprite::PositionIndex(x, y));
            auto ball = (BallSprite*)(getChildByTag(tag));
            
            if(ball)
            {
                float distance = ball->getPosition().getDistance(touchPos);
                
                if(distance <= BALL_SIZE / 2)
                {
                    return ball;
                }
            }
        }
    }
    
    return nullptr;
}

void GameLayer::checksLinedBalls()
{
    _touchable = false;
    
    //３個以上同じボールが並んだ時
    if(existsLinedBalls())
    {
        _chainNumber++;
        removeAndGenerateBalls();
        
        auto delay = DelayTime::create(ONE_ACTION_TIME * (_maxRemovedNo + 1));
        auto func = CallFunc::create(CC_CALLBACK_0(GameLayer::checksLinedBalls,  this));
        auto seq = Sequence::create(delay, func, nullptr);
        runAction(seq);
    }
    else
    {
        _touchable = true;
    }
}

bool GameLayer::existsLinedBalls()
{
    initBallParms();
    
    _maxRemovedNo = 0;
    
    for(int x = 1; x <= BALL_NUM_X; x++)
    {
        for(int y = 1; y <= BALL_NUM_Y; y++)
        {
            checkedBall(BallSprite::PositionIndex(x, y), Direction::x);
            checkedBall(BallSprite::PositionIndex(x, y), Direction::y);
        }
    }
    
    return _maxRemovedNo > 0;
}

//全てのボールからballTypeを取得
Map<int, BallSprite*> GameLayer::getAllBalls()
{
    auto balls = Map<int, BallSprite*>();
    
    for(auto object : getChildren())
    {
        auto ball = dynamic_cast<BallSprite*>(object);
        if(ball)
        {
            balls.insert(ball->getTag(), ball);
        }
    }
    
    return balls;
}

//指定した方向のボールが同じ色かチェック
bool GameLayer::isSameBallType(BallSprite::PositionIndex current, Direction direction)
{
    auto allBalls = getAllBalls();
    
    //x方向でチェック
    if(direction == Direction::x)
    {
        if(current.x + 1 > BALL_NUM_X)
        {
            return false;
        }
    }
    //y方向でチェック
    else
    {
        if(current.y + 1 > BALL_NUM_Y)
        {
            return false;
        }
        
    }
    
    int currentTag = BallSprite::generateTag(BallSprite::PositionIndex (current.x, current.y));
    BallSprite* currentBall = allBalls.at(currentTag);
    
    int nextTag;
    if(direction == Direction::x)
    {
        nextTag = BallSprite::generateTag(BallSprite::PositionIndex (current.x + 1, current.y));
    }
    else
    {
        nextTag = BallSprite::generateTag(BallSprite::PositionIndex (current.x, current.y + 1));
    }
    auto nextBall = allBalls.at(nextTag);
    
    //現在のボールと隣のボールが同じタイプのボールかチェック
    if(currentBall->getBallType() == nextBall->getBallType())
    {
        return true;
    }
    
    return false;
}

void GameLayer::initBallParms()
{
    auto allBalls = getAllBalls();
    for(auto ball : allBalls)
    {
        ball.second->resetParams();
    }
}

void GameLayer::checkedBall(BallSprite::PositionIndex current, Direction direction)
{
    auto allBalls = getAllBalls();
    
    int tag = BallSprite::generateTag(BallSprite::PositionIndex (current.x, current.y));
    
    BallSprite* ball = allBalls.at(tag);
 
    bool checked;
    
    if(direction == Direction::x)
    {
        checked = ball->getCheckedX();
    }
    else
    {
        checked = ball->getCheckedY();
    }
    
    if(!checked)
    {
        int num = 0;
        
        while(true)
        {
            BallSprite::PositionIndex searchPosition;
            if(direction == Direction::x)
            {
                searchPosition = BallSprite::PositionIndex(current.x + num, current.y);
            }
            else
            {
                searchPosition = BallSprite::PositionIndex(current.x, current.y + num);
            }
            
            if(isSameBallType(searchPosition, direction))
            {
                int nextTag = BallSprite::generateTag(searchPosition);
                auto nextBall = allBalls.at(nextTag);
                
                if(direction == Direction::x)
                {
                    nextBall->setCheckedX(true);
                }
                else
                {
                    nextBall->setCheckedY(true);
                }
                
                num++;
            }
            else
            {
                if(num >= 2)
                {
                    int removedNo = 0;
                    
                    if(_removeNumbers.size() <= _chainNumber)
                    {
                        std::map<BallSprite::BallType, int> removedNumber;
                        _removeNumbers.push_back(removedNumber);
                    }
                    _removeNumbers[_chainNumber][ball->getBallType()] += num + 1;
                    //_removeNumbers[_chainNumber][ball->getBallType()] = _removeNumbers[_chainNumber][ball->getBallType()] + num + 1;
                    
                    for(int i = 0; i <= num; i++)
                    {
                        BallSprite::PositionIndex linePosition;
                        if(direction == Direction::x)
                        {
                            linePosition = BallSprite::PositionIndex(current.x + i, current.y);
                        }
                        else
                        {
                            linePosition = BallSprite::PositionIndex(current.x, current.y + i);
                        }
                        
                        int lineBallTag = BallSprite::generateTag(linePosition);
                        auto linedBall = allBalls.at(lineBallTag);
                        
                        if(linedBall->getRemovedNo() > 0)
                        {
                            removedNo = linedBall->getRemovedNo();
                            break;
                        }
                    }
                    
                    if(removedNo == 0)
                    {
                        removedNo = ++_maxRemovedNo;
                        //removedNo = 1 + _maxRemovedNo;
                    }
                    
                    for(int i = 0; i <= num; i++)
                    {
                        BallSprite::PositionIndex linePosition;
                        if(direction == Direction::x)
                        {
                            linePosition = BallSprite::PositionIndex(current.x + i, current.y);
                        }
                        else
                        {
                            linePosition = BallSprite::PositionIndex(current.x, current.y + i);
                        }
                        
                        int lineBallTag = BallSprite::generateTag(linePosition);
                        auto linedBall = allBalls.at(lineBallTag);
                        
                        linedBall->setRemovedNo(removedNo);
                    }
                }
                break;
            }
        };
        
        if(direction == Direction::x)
        {
            ball->setCheckedX(true);
        }
        else
        {
            ball->setCheckedY(true);
        }
    }
}

void GameLayer::animationBalls()
{
    auto allBalls = getAllBalls();
    
    for(auto ball : allBalls)
    {
        ball.second->removingAndFallingAnimation(_maxRemovedNo);
    }
}

void GameLayer::removeAndGenerateBalls()
{
    auto allBalls = getAllBalls();
    
    int maxRemovedNo = 0;
    
    for(int x = 1; x <= BALL_NUM_X; x++)
    {
        int fallCount = 0;
        
        for(int y = 1; y <= BALL_NUM_Y; y++)
        {
            int tag = BallSprite::generateTag(BallSprite::PositionIndex(x, y));
            
            auto ball = allBalls.at(tag);
            
            if(ball)
            {
                int removedNoForBall = ball->getRemovedNo();
                
                if(removedNoForBall > 0)
                {
                    fallCount++;
                    
                    if(removedNoForBall > maxRemovedNo)
                    {
                        maxRemovedNo = removedNoForBall;
                    }
                }
                else
                {
                    ball->setFallCount(fallCount);
                }
            }
        }
        
        generateBalls(x, fallCount);
    }
    
    animationBalls();
}

void GameLayer::generateBalls(int xlineNum, int fallCount)
{
    for(int i = 1; i <= fallCount; i++)
    {
        auto positionindex = BallSprite::PositionIndex(xlineNum, BALL_NUM_Y + i);
        auto ball = newBalls(positionindex, false);
        
        ball->setFallCount(fallCount);
    }
}


//ボール操作終了処理
void GameLayer::movedBall()
{
    _movingBall->resetPosition();
    _movingBall = nullptr;
    
    _chainNumber = 0;
    _removeNumbers.clear();
    checksLinedBalls();
}

/*各種タッチイベントの処理*/

bool GameLayer::onTouchBegan(Touch* touch, Event* unused_event)
{
    if(!_touchable)
    {
        return false;
    }
    
    _movedBall = false;
    _movingBall = getTouchBall(touch->getLocation());
    
    if(_movingBall)
    {
        return true;
    }
    else{
        return false;
    }
}

//ボールの移動中処理
//touchBall:移動させているボール
void GameLayer::onTouchMoved(Touch* touch, Event* unused_event)
{
    _movingBall->setPosition(_movingBall->getPosition() + touch->getDelta());
    
    auto touchBall = getTouchBall(touch->getLocation(),
                                  _movingBall->getPositionIndex());
    
    if(touchBall && _movingBall != touchBall)
    {
        _movedBall = true;
        
        auto touchBallPositionIndex = touchBall->getPositionIndex();
        touchBall->setPositionIndexAndChangePosition(_movingBall->getPositionIndex());
        
        _movingBall->setPositionIndex(touchBallPositionIndex);
    }
}

void GameLayer::onTouchEnded(Touch* touch, Event* unused_event)
{
    movedBall();
}

void GameLayer::onTouchCancelled(Touch* touch, Event* unused_event)
{
    onTouchEnded(touch, unused_event);
}

