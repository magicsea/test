#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include "Box2D/Box2D.h"

#include "SimpleAudioEngine.h"

#include "GLES-Render.h"
#include "RayCastCallback.h"
#include "CCBlade.h"
#include <math.h>

#define PTM_RATIO 32
#define calculate_determinant_2x2(x1,y1,x2,y2) (x1 * y2 - y1 * x2)
#define calculate_determinant_2x3(x1,y1,x2,y2,x3,y3) (x1 * y2 + x2 * y3 + x3 * y1-y1 * x2 - y2 * x3 - y3 * x1)

#define frandom_range(low,high) ((high-low)*CCRANDOM_0_1())+low
#define random_range(low,high) (rand()%(high-low+1))+low

#define midpoint(a, b) (float)(a + b)/2

typedef enum _TossType
{
	kTossConsecutive = 0,
	kTossSimultaneous
}TossType;

class HelloWorld : public cocos2d::CCLayer
{
public:
	~HelloWorld();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();

	virtual void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);

	void draw();
	void update(cocos2d::ccTime dt);

	b2Vec2* arrangeVertices(b2Vec2* vertices, int count);
	bool splitPolygonSprite(PolygonSprite* sprite);
	bool areVerticesAcceptable(b2Vec2* vertices, int count);
	b2Body* createBodyX(b2Vec2 position, float rotation, b2Vec2* vertices, int32 count, float density, float friction, float restitution);
    
    // implement the "static node()" method manually
    LAYER_NODE_FUNC(HelloWorld);

	CC_SYNTHESIZE_RETAIN(cocos2d::CCArray*, _blades, Blades);
	CC_SYNTHESIZE_RETAIN(cocos2d::CCArray*, _cache, Cache);

private:
	b2World* world;
	GLESDebugDraw* m_debugDraw;

	cocos2d::CCPoint _startPoint;
	cocos2d::CCPoint _endPoint;
	RaycastCallback* _raycastCallback;

	CCBlade* _blade;
	float _deltaRemainder;

	double _nextTossTime;
	double _tossInterval;
	int _queuedForToss;
	TossType _currentTossType;

	int _cuts;
	int _lives;
	cocos2d::CCLabelTTF* _cutsLabel;

	cocos2d::CCParticleSystemQuad* _bladeSparkle;

	float _timeCurrent;
	float _timePrevious;

	void initPhysics();
	void initSprites();
	void initBackground();
	void initHUD();

	void addNewSpriteAtPosition(cocos2d::CCPoint p);
	void createMenu();

	void checkAndSliceObjects();
	void clearSlices();

	void tossSprite(PolygonSprite* sprite);
	void spriteLoop();
	void cleanSprites();

	void restart(cocos2d::CCObject* pSender);
	void endGame();
	void subtractLife();

	long millisecondNow(){
		cocos2d::cc_timeval now;
		cocos2d::CCTime::gettimeofdayCocos2d(&now, NULL);
		return (now.tv_sec * 1000 + now.tv_usec / 1000);
	}

};


#endif  // __HELLOWORLD_SCENE_H__