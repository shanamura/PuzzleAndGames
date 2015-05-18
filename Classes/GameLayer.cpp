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
#define WINSIZE Director::getInstance()->getWinSize()

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
    _distForMember = std::uniform_int_distribution<int>(0, 4);
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
    initEnemy();
    initMenbers();
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
    //キャラ画面用背景
    auto bgForCharacter = Sprite::create("Background1.png");
    bgForCharacter->setAnchorPoint(Point(0, 1));
    bgForCharacter->setPosition(Point(0, WINSIZE.height));
    
    addChild(bgForCharacter, BgForCharacter);

    //パズル画面用背景
    auto bgForPuzzle = Sprite::create("Background2.png");
    bgForPuzzle->setAnchorPoint(Point::ZERO);
    bgForPuzzle->setPosition(Point::ZERO);
    
    addChild(bgForPuzzle, BgForPuzzle);
}

//敵の表示
void GameLayer::initEnemy()
{
    _enemyData = Character::create();
    _enemyData->retain();
    _enemyData->setMaxHp(10000);    //最大HP
    _enemyData->setHp(10000);       //現在のHP
    _enemyData->setElement(Character::Element::Wind);
    _enemyData->setTurnCount(3);
    
    _enemy = Sprite::create("Enemy1.png");
    _enemy->setPosition(Point(320, 660 + (WINSIZE.height - 660) / 2));
    addChild(_enemy, Enemy);
    
    //HPの表示
    auto hpBg = Sprite::create("HpEnemyBackground.png");
    hpBg->setPosition(Point(320, 530 + (WINSIZE.height - 660) / 2));
    addChild(hpBg, EnemyHp);
    
    //HPバーの表示
    _hpBarForEnemy = ProgressTimer::create(Sprite::create("HpEnemyRed.png"));
    _hpBarForEnemy->setPosition(Point(hpBg->getContentSize().width / 2,
                                      hpBg->getContentSize().height / 2));
    _hpBarForEnemy->setType(ProgressTimer::Type::BAR);
    _hpBarForEnemy->setMidpoint(Point::ZERO);
    _hpBarForEnemy->setBarChangeRate(Point(1, 0));
    _hpBarForEnemy->setPercentage(_enemyData->getHpPercentage());
    
    hpBg->addChild(_hpBarForEnemy);
}

//メンバーの表示
void GameLayer::initMenbers()
{
    std::vector<std::string> fileNames
    {
        "CardBlue.png",
        "CardRed.png",
        "CardGreen.png",
        "CardYellow.png",
        "CardPurple.png"
    };
    
    std::vector<Character::Element> elements
    {
        Character::Element::Water,
        Character::Element::Fire,
        Character::Element::Wind,
        Character::Element::Holy,
        Character::Element::Shadow
    };
    
    for(int i = 0; i < fileNames.size(); i++)
    {
        auto memberData = Character::create();
        memberData->setMaxHp(200);              //最大HP
        memberData->setHp(200);                 //現在のHP
        memberData->setElement(elements[i]);
        _memberDatum.pushBack(memberData);
        
        auto member = Sprite::create(fileNames[i].c_str());
        member->setPosition(Point(70 + i * 125, 598));
        addChild(member, Char);
        
        //HPの表示
        auto hpBg = Sprite::create("HpCardBackground.png");
        hpBg->setPosition(Point(70 + i * 125, 554));
        addChild(hpBg, CharHp);
        
        //HPバーの表示
        auto hpBarForMember = ProgressTimer::create(Sprite::create("HpCardGreen.png"));
        hpBarForMember->setPosition(Point(hpBg->getContentSize().width / 2,
                                          hpBg->getContentSize().height / 2));
        hpBarForMember->setType(ProgressTimer::Type::BAR);
        hpBarForMember->setMidpoint(Point::ZERO);
        hpBarForMember->setBarChangeRate(Point(1, 0));
        hpBarForMember->setPercentage(memberData->getHpPercentage());
        hpBg->addChild(hpBarForMember);
        
        _members.pushBack(member);
        _hpBarForMembers.pushBack(hpBarForMember);
    }
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
        int chainNum = 0;
        int damage = 0;
        int healing = 0;
        std::set<int> attackers;
        
        //ダメージ量と回復量の計算
        calculateDamage(chainNum, healing, damage, attackers);
        
        int afterHp = _enemyData->getHp() - damage;
        
        
        if(damage > 0)
        {
            attackToEnemy(damage, attackers);
        }
        
        if(healing > 0)
        {
            healMember(healing);
        }
        
        if(afterHp > 0)
        {
            CallFunc* func = CallFunc::create(CC_CALLBACK_0(GameLayer::attackFromEnemy, this));
            runAction(Sequence::create(DelayTime::create(0.5), func, nullptr));
        }
        
        //_touchable = true;
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

//ダメージ計算
void GameLayer::calculateDamage(int &chainNum, int &healing, int &damage, std::set<int> &attackers)
{
    auto removeIt = _removeNumbers.begin();
    
    while (removeIt != _removeNumbers.end())
    {
        auto ballIt = (*removeIt).begin();
        while (ballIt != (*removeIt).end())
        {
            if((*ballIt).first == BallSprite::BallType::Pink)
            {
                healing = healing + 5;
            }
            else
            {
                for(int i = 0; i < _memberDatum.size(); i++)
                {
                    auto memberData = _memberDatum.at(i);
                    
                    if(memberData->getHp() <= 0)
                    {
                        continue;
                    }
                    
                    if(isAttacker((*ballIt).first, memberData->getElement()))
                    {
                        attackers.insert(i);
                        damage = damage + Character::getDamage((*ballIt).second, chainNum, memberData, _enemyData);
                    }
                }
            }
            
            chainNum++;
            ballIt++;
        }
        
        removeIt++;
    }
}

//アタッカー判定
bool GameLayer::isAttacker(BallSprite::BallType type, Character::Element element)
{
    switch (type) {
        case BallSprite::BallType::Red:
        {
            if(element == Character::Element::Fire)
            {
                return true;
            }
            break;
        }
            
        case BallSprite::BallType::Blue:
        {
            if(element == Character::Element::Water)
            {
                return true;
            }
            break;
        }
            
        case BallSprite::BallType::Green:
        {
            if(element == Character::Element::Wind)
            {
                return true;
            }
            break;
        }
            
        case BallSprite::BallType::Yellow:
        {
            if(element == Character::Element::Holy)
            {
                return true;
            }
            break;
        }
            
        case BallSprite::BallType::Purple:
        {
            if(element == Character::Element::Shadow)
            {
                return true;
            }
            break;
        }
            
        default:
            break;
    }
    
    return false;
}

//敵への攻撃
void GameLayer::attackToEnemy(int damage, std::set<int> attackers)
{
    float preHpPrecentage = _enemyData->getHpPercentage();
    
    int afterHp = _enemyData->getHp() - damage;
    if(afterHp < 0)
    {
        afterHp = 0;
    }
    
    _enemyData->setHp(afterHp);
    
    auto act = ProgressFromTo::create(0.5, preHpPrecentage, _enemyData->getHpPercentage());
    _hpBarForEnemy->runAction(act);
    
    _enemy->runAction(vibratingAnimation(afterHp));
    
    for(auto attacker : attackers)
    {
        auto member = _members.at(attacker);
        member->runAction(Sequence::create(MoveBy::create(0.1, Point(0, 10)),
                                           MoveBy::create(0.1, Point(0, -10)),nullptr));
    }
}

//回復処理
void GameLayer::healMember(int healing)
{
    for (int i = 0; i < _memberDatum.size(); i++)
    {
        auto memberData = _memberDatum.at(i);
        
        if(memberData->getHp() <= 0)
        {
            continue;
        }
        
        float preHpPrecentage = memberData->getHpPercentage();
        int afterHp = memberData->getHp() + healing;
        
        if(afterHp > memberData->getMaxHp())
        {
            afterHp = memberData->getMaxHp();
        }
        
        memberData->setHp(afterHp);
        
        auto act = ProgressFromTo::create(0.5, preHpPrecentage, memberData->getHpPercentage());
        _hpBarForMembers.at(i)->runAction(act);
    }
}

//敵からの攻撃
void GameLayer::attackFromEnemy()
{
    if(!_enemyData->isAttackturn())
    {
        endAnimation();
        return;
    }
    
    int index;
    Character* memberData;

    do{
        index = _distForMember(_engine);
        memberData = _memberDatum.at(index);
    }while (memberData->getHp() <= 0);
    
    auto member = _members.at(index);
    auto hpBarForMember = _hpBarForMembers.at(index);
    
    float preHpPrecentage = memberData->getHpPercentage();
    int afterHp = memberData->getHp() - 25;
    
    if(afterHp > memberData->getMaxHp())
    {
        afterHp = memberData->getMaxHp();
    }
    memberData->setHp(afterHp);
    
    
    auto act = ProgressFromTo::create(0.5, preHpPrecentage, memberData->getHpPercentage());
    hpBarForMember->runAction(act);
    
    member->runAction(vibratingAnimation(afterHp));
    
    auto seq = Sequence::create(MoveBy::create(0.1, Point(0, -10)),
                                MoveBy::create(0.1, Point(0, 10)), nullptr);
    _enemy->runAction(seq);
    
    
    //全滅チェック
    bool allMemberHpZero = true;
    
    for(auto character : _memberDatum)
    {
        if(character->getHp() > 0)
        {
            allMemberHpZero = false;
            break;
        }
    }
    
    
    if(!allMemberHpZero)
    {
        CallFunc* func = CallFunc::create(CC_CALLBACK_0(GameLayer::endAnimation, this));
        runAction(Sequence::create(DelayTime::create(0.5), func, nullptr));
    }
}

void GameLayer::endAnimation()
{
    _touchable = true;
}

cocos2d::Spawn* GameLayer::vibratingAnimation(int afterHp)
{
    auto move = Sequence::create(MoveBy::create(0.025, Point(5, 5)),
                                 MoveBy::create(0.025, Point(-5, -5)),
                                 MoveBy::create(0.025, Point(-5, -5)),
                                 MoveBy::create(0.025, Point(5, 5)),
                                 MoveBy::create(0.025, Point(5, -5)),
                                 MoveBy::create(0.025, Point(-5, 5)),
                                 MoveBy::create(0.025, Point(-5, 5)),
                                 MoveBy::create(0.025, Point(5, -5)),
                                nullptr);
    
    Action* tint;
    if(afterHp > 0)
    {
        tint = Sequence::create(TintTo::create(0, 255, 0, 0),
                                DelayTime::create(0.2),
                                TintTo::create(0, 255, 255, 255),
                                nullptr);
    }
    else
    {
        tint = TintTo::create(0, 255, 0, 0);
    }
    
    return Spawn::create(move, tint, nullptr);
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

