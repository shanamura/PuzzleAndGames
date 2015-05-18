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
#include "Character.h"
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
    
    std::uniform_int_distribution<int> _distForMember;
    
    BallSprite* _movingBall;
    bool _movedBall;
    
    bool _touchable;
    int _maxRemovedNo;
    int _chainNumber;
    std::vector<std::map<BallSprite::BallType, int>> _removeNumbers;
    
    //エネミー情報
    Character* _enemyData;
    cocos2d::Sprite* _enemy;
    cocos2d::ProgressTimer* _hpBarForEnemy;
    
    //メンバー情報
    cocos2d::Vector<Character*> _memberDatum;
    cocos2d::Vector<cocos2d::Sprite*> _members;
    cocos2d::Vector<cocos2d::ProgressTimer*> _hpBarForMembers;
    
    //ステージレベル
    int _level;
    int _nextLevel;
    
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
    
    
    //キャラの描画処理
    void initEnemy();
    void initMenbers();
    
    /*ダメージ計算とアニメーション関する処理*/
    void calculateDamage(int &chainNum, int &healing, int &damage, std::set<int> &attackers);
    bool isAttacker(BallSprite::BallType type, Character::Element element);
    void attackToEnemy(int damage, std::set<int> attackers);
    void healMember(int healing);
    void attackFromEnemy();
    void endAnimation();
    cocos2d::Spawn* vibratingAnimation(int afterHp);
    
    /*ステージレベル表示に関する処理*/
    void initLevelLayer();
    void removeLevelLayer(float dt);
    void winAnimation();
    void loseAnimation();
    void nextScene(float dt);
    
    
public:
    GameLayer();
    virtual bool init(int level);
    //CREATE_FUNC(GameLayer);
    static GameLayer* create(int level);
    static cocos2d::Scene* createScene(int level = 1);

    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* unused_event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* unused_event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* unused_event);
    virtual void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* unused_event);
};


#endif /* defined(__PuzzleAndGames__GameLayer__) */
