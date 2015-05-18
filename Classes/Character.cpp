//
//  Character.cpp
//  PuzzleAndGames
//
//  Created by hashi on 2015/05/18.
//
//

#include "Character.h"

USING_NS_CC;

Character::Character()
: _remainingTurn(0)
, _hp(0)
, _attack(0)
, _element(Element::None)
, _turnCount(0)
{
}

Character* Character::create()
{
    Character *pRet = new Character();
    pRet->autorelease();
    
    return pRet;
}

//CC_PROPERTY(int, _turnCount, TurnCount)
//get,setメソッドを定義
int Character::getTurnCount()
{
    return _turnCount;
}

void Character::setTurnCount(int turnCount)
{
    _turnCount = turnCount;
    _remainingTurn = turnCount;
}

//ヒットポイントを%割合で取得
float Character::getHpPercentage(){
    return _hp * 100.f / _maxHp;
}


bool Character::isAttackturn()
{
    _remainingTurn--;
    
    if(_remainingTurn <= 0)
    {
        _remainingTurn = _turnCount;
        return true;
    }
    
    return false;
}

int Character::getDamage(int ballCount, int chainCount,
                                Character *attacker, Character *defender)
{
    //基本ダメージ値の計算
    float baseDamage = ballCount / 3.0 * 100;

    float chainBonus = powf(1.1, chainCount);
    float elementBonus = getElementBonus(attacker->getElement(),
                                         defender->getElement());
    
    return baseDamage * chainBonus * elementBonus;
}

//属性計算
float Character::getElementBonus(Element attackElement, Element defenseElement)
{
    switch (attackElement) {
        case Fire:
        {
            switch (defenseElement) {
                case Wind:
                    return 2.0;
                case Water:
                    return 0.5;
                default:
                    return 1.0;
            }
        }
            
        case Water:
        {
            switch (defenseElement) {
                case Fire:
                    return 2.0;
                case Wind:
                    return 0.5;
                default:
                    return 1.0;
            }
        }
            
        case Wind:
        {
            switch (defenseElement) {
                case Water:
                    return 2.0;
                case Fire:
                    return 0.5;
                default:
                    return 1.0;
           }
        }
            
        case Holy:
        {
            switch (defenseElement) {
                case Shadow:
                    return 2.0;
                default:
                    return 1.0;
           }
        }
            
        case Shadow:
        {
            switch (defenseElement) {
                case Holy:
                    return 2.0;
                default:
                    return 1.0;
           }
        }
            
        default:
        {
            return 1.0;
        }
    }
}


