//
//  Character.h
//  PuzzleAndGames
//
//  Created by hashi on 2015/05/18.
//
//

#ifndef __PuzzleAndGames__Character__
#define __PuzzleAndGames__Character__

#include "cocos2d.h"

class Character : public cocos2d::Ref
{
public:
    
    enum Element
    {
        Fire,
        Water,
        Wind,
        Holy,
        Shadow,
        None
    };
    
    Character();    //コンストラクタ
    static Character* create();
    
    float getHpPercentage();
    bool isAttackturn();
    
    //ダメージ取得
    static int getDamage(int ballCount, int chainCount, Character* attacker, Character* defender);
    
protected:
    int _remainingTurn;
    CC_SYNTHESIZE(int, _hp, Hp);
    CC_SYNTHESIZE(int, _maxHp, MaxHp);
    CC_SYNTHESIZE(int, _attack, Attack);
    CC_SYNTHESIZE(Element, _element, Element);
    CC_PROPERTY(int, _turnCount, TurnCount);

    //属性ボーナス
    static float getElementBonus(Element attackElement, Element defenseElement);
};


#endif /* defined(__PuzzleAndGames__Character__) */
