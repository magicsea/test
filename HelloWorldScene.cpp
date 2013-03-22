#include "HelloWorldScene.h"
#include "Watermelon.h"
#include "Bomb.h"
#include "Strawberry.h"
#include "Pineapple.h"
#include "Grapes.h"
#include "Banana.h"

using namespace cocos2d;
using namespace CocosDenshion;


enum {
	kTagParentNode = 1,
};

int comparator(const void *a, const void *b){
	const b2Vec2 *va = (const b2Vec2 *)a;
	const b2Vec2 *vb = (const b2Vec2 *)b;

	if (va->x > vb->x) {
		return 1;
	} else if (va->x < vb->x) {
		return -1;
	}
	return 0;   
};

HelloWorld::~HelloWorld(){
	delete world;
	world = NULL;

	delete m_debugDraw;
	m_debugDraw = NULL;

	delete _raycastCallback;
	_raycastCallback = NULL;

	CC_SAFE_RELEASE_NULL(_cache);
	CC_SAFE_RELEASE_NULL(_blades);

}

CCScene* HelloWorld::scene()
{
    CCScene * scene = NULL;
    do 
    {
        // 'scene' is an autorelease object
        scene = CCScene::node();
        CC_BREAK_IF(! scene);

        // 'layer' is an autorelease object
        HelloWorld *layer = HelloWorld::node();
        CC_BREAK_IF(! layer);

        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    bool bRet = false;
    do 
    {
        //////////////////////////////////////////////////////////////////////////
        // super init first
        //////////////////////////////////////////////////////////////////////////

        CC_BREAK_IF(! CCLayer::init());

        //////////////////////////////////////////////////////////////////////////
        // add your codes below...
        //////////////////////////////////////////////////////////////////////////

        this->setIsTouchEnabled(true);

		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		this->initPhysics();
		this->initSprites();
		this->initHUD();
		this->initBackground();
		_raycastCallback = new RaycastCallback;

		_deltaRemainder = 0.0f;
		_blade = NULL;
		_blades = new CCArray;
		_blades->initWithCapacity(3);
		CCTexture2D* texture = CCTextureCache::sharedTextureCache()->addImage("streak.png");
		for(int i = 0; i < 3; i++){
			CCBlade* blade = CCBlade::bladeWithMaximumPoint(50);
			blade->setAutoDim(false);
			blade->setTexture(texture);

			this->addChild(blade, 2);
			_blades->addObject(blade);
		}

		_nextTossTime = millisecondNow() + 1000;
		_queuedForToss = 0;

		_bladeSparkle = CCParticleSystemQuad::particleWithFile("blade_sparkle.plist");
		_bladeSparkle->stopSystem();
		this->addChild(_bladeSparkle, 3);

		SimpleAudioEngine::sharedEngine()->preloadEffect("swoosh.mp3");
		SimpleAudioEngine::sharedEngine()->preloadEffect("toss_consecutive.mp3");
		SimpleAudioEngine::sharedEngine()->preloadEffect("toss_simultaneous.mp3");
		SimpleAudioEngine::sharedEngine()->preloadEffect("toss_bomb.mp3");
		SimpleAudioEngine::sharedEngine()->preloadEffect("lose_life.mp3");
		SimpleAudioEngine::sharedEngine()->preloadEffect("squash.mp3");
		SimpleAudioEngine::sharedEngine()->playBackgroundMusic("nature_bgm.mp3");
		_timeCurrent = 0;
		_timePrevious = 0;

		srand((unsigned int)time(NULL));

		this->scheduleUpdate();

        bRet = true;
    } while (0);

    return bRet;
}

void HelloWorld::initPhysics(){

	CCSize s = CCDirector::sharedDirector()->getWinSize();

	b2Vec2 gravity;
	gravity.Set(0.0f, -4.25f);
	world = new b2World(gravity);


	// Do we want to let bodies sleep?
	world->SetAllowSleeping(true);

	world->SetContinuousPhysics(true);

	m_debugDraw = new GLESDebugDraw( PTM_RATIO );
	world->SetDebugDraw(m_debugDraw);

	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	//		flags += b2Draw::e_jointBit;
	//		flags += b2Draw::e_aabbBit;
	//		flags += b2Draw::e_pairBit;
	//		flags += b2Draw::e_centerOfMassBit;
	m_debugDraw->SetFlags(flags);		

	/*
	// Define the ground body.
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0, 0); // bottom-left corner

	// Call the body factory which allocates memory for the ground body
	// from a pool and creates the ground box shape (also from a pool).
	// The body is also added to the world.
	b2Body* groundBody = world->CreateBody(&groundBodyDef);

	// Define the ground box shape.
	b2EdgeShape groundBox;		

	// bottom

	groundBox.Set(b2Vec2(0,0), b2Vec2(s.width/PTM_RATIO,0));
	groundBody->CreateFixture(&groundBox,0);

	// top
	groundBox.Set(b2Vec2(0,s.height/PTM_RATIO), b2Vec2(s.width/PTM_RATIO,s.height/PTM_RATIO));
	groundBody->CreateFixture(&groundBox,0);

	// left
	groundBox.Set(b2Vec2(0,s.height/PTM_RATIO), b2Vec2(0,0));
	groundBody->CreateFixture(&groundBox,0);

	// right
	groundBox.Set(b2Vec2(s.width/PTM_RATIO,s.height/PTM_RATIO), b2Vec2(s.width/PTM_RATIO,0));
	groundBody->CreateFixture(&groundBox,0);
	*/
}

void HelloWorld::initSprites(){	
	_cache = new CCArray;
	if(_cache && _cache->initWithCapacity(53)){


		for(int i = 0; i < 10; i++){
			Watermelon* sprite = Watermelon::wmWithWorld(world);
			sprite->setPosition(ccp(-64*(i + 1), -64));
			this->addChild(sprite, 1);
			this->addChild(sprite->getSplurt(), 3);
			_cache->addObject(sprite);
		}

		for(int i = 0; i < 10; i++){
			Strawberry* sprite = Strawberry::strawberryWithWorld(world);
			sprite->setPosition(ccp(-64*(i + 1), -64));
			this->addChild(sprite, 1);
			this->addChild(sprite->getSplurt(), 3);
			_cache->addObject(sprite);
		}

		for(int i = 0; i < 10; i++){
			Pineapple* sprite = Pineapple::appleWithWorld(world);
			sprite->setPosition(ccp(-64*(i + 1), -64));
			this->addChild(sprite, 1);
			this->addChild(sprite->getSplurt(), 3);
			_cache->addObject(sprite);
		}

		for(int i = 0; i < 10; i++){
			Grapes* sprite = Grapes::grapesWithWorld(world);
			sprite->setPosition(ccp(-64*(i + 1), -64));
			this->addChild(sprite, 1);
			this->addChild(sprite->getSplurt(), 3);
			_cache->addObject(sprite);
		}

		for(int i = 0; i < 10; i++){
			Banana* sprite = Banana::bananaWithWorld(world);
			sprite->setPosition(ccp(-64*(i + 1), -64));
			this->addChild(sprite, 1);
			this->addChild(sprite->getSplurt(), 3);
			_cache->addObject(sprite);
		}

		for(int i = 0; i < 3; i++){
			Bomb* sprite = Bomb::bombWithWorld(world);
			sprite->setPosition(ccp(-64*(i + 1), -64));
			this->addChild(sprite, 1);
			this->addChild(sprite->getSplurt(), 3);
			_cache->addObject(sprite);
		}

	}

}

void HelloWorld::initHUD(){
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	_cuts = 0;
	_lives = 3;

	for(int i = 0; i < 3; i++){
		CCSprite* cross = CCSprite::spriteWithFile("x_unfilled.png");
		cross->setPosition(ccp(winSize.width - cross->getContentSize().width * 0.5f - i * cross->getContentSize().width,
			winSize.height - cross->getContentSize().height * 0.5f));
		this->addChild(cross, 4);
	}

	CCSprite* cutsIcon = CCSprite::spriteWithFile("fruit_cut.png");
	cutsIcon->setPosition(ccp(cutsIcon->getContentSize().width * 0.5f, winSize.height - cutsIcon->getContentSize().height * 0.5f));
	this->addChild(cutsIcon, 4);

	_cutsLabel  = CCLabelTTF::labelWithString("0", "Helvetica Neue", 30);
	_cutsLabel->setAnchorPoint(ccp(0.0f, 0.5f));
	_cutsLabel->setPosition(ccp(cutsIcon->getContentSize().width + _cutsLabel->getContentSize().width * 0.5f, cutsIcon->getPosition().y));
	this->addChild(_cutsLabel, 4);
}

void HelloWorld::initBackground(){
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	CCSprite* background = CCSprite::spriteWithFile("bg.png");
	background->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f));
	this->addChild(background, 0);

	CCParticleSystemQuad* sunPollen = CCParticleSystemQuad::particleWithFile("sun_pollen.plist");
	this->addChild(sunPollen);
}


void HelloWorld::draw(){
	CCLayer::draw();

	ccGLEnableVertexAttribs(kCCVertexAttribFlag_Position);
	kmGLPushMatrix();

	//ccDrawLine(_startPoint, _endPoint);

	//world->DrawDebugData();

	kmGLPopMatrix();
}

void HelloWorld::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent){
	CCSetIterator iter = pTouches->begin();
	for(; iter != pTouches->end(); iter++){
		CCTouch* pTouch = (CCTouch*)*iter;
		CCPoint location = CCDirector::sharedDirector()->convertToGL(pTouch->locationInView());

		_startPoint = location;
		_endPoint = location;

		_bladeSparkle->setPosition(location);
		_bladeSparkle->resetSystem();

		CCObject* temp;
		CCARRAY_FOREACH(_blades, temp){
			CCBlade* blade = (CCBlade*)temp;
			if(blade->getPath()->count() == 0){
				_blade = blade;
				_blade->push(location);
				break;
			}
		}
	}
}

void HelloWorld::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent){
	CCSetIterator iter = pTouches->begin();
	for(; iter != pTouches->end(); iter++){
		CCTouch* pTouch = (CCTouch*)*iter;
		CCPoint location = CCDirector::sharedDirector()->convertToGL(pTouch->locationInView());

		_endPoint = location;

		ccTime deltaTime = _timeCurrent - _timePrevious;
		_timePrevious = _timeCurrent;

		_bladeSparkle->setPosition(location);

		if(ccpLengthSQ(ccpSub(_startPoint, _endPoint)) > 25){
			world->RayCast(_raycastCallback, b2Vec2(_startPoint.x / PTM_RATIO, _startPoint.y / PTM_RATIO),
				b2Vec2(_endPoint.x / PTM_RATIO, _endPoint.y / PTM_RATIO));
			world->RayCast(_raycastCallback,b2Vec2(_endPoint.x / PTM_RATIO, _endPoint.y / PTM_RATIO),
				b2Vec2(_startPoint.x / PTM_RATIO, _startPoint.y / PTM_RATIO));

			_startPoint = _endPoint;
		}

		_blade->push(location);
	}
}

void HelloWorld::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent){
	_bladeSparkle->stopSystem();

	clearSlices();
	_blade->dim(true);
}

void HelloWorld::update(cocos2d::ccTime dt){

	int32 velocityIterations = 8;
	int32 positionIterations = 1;

	world->Step(dt, velocityIterations, positionIterations);	

	this->checkAndSliceObjects();
	this->cleanSprites();

	
	if(_blade && _blade->getPath()->count() > 3){
		_deltaRemainder = dt * 60 * 1.2f;
		int pop = (int)(_deltaRemainder + 0.5f);
		_deltaRemainder -= pop;
		_blade->pop(pop);
	}

	this->spriteLoop();

	_timeCurrent += dt;
}

bool HelloWorld::splitPolygonSprite(PolygonSprite* sprite){
	bool flag = false;

	PolygonSprite* newSprite1;
	PolygonSprite* newSprite2;

	b2Fixture* originalFixture = sprite->getBody()->GetFixtureList();
	b2PolygonShape* originalPolygon = (b2PolygonShape*)originalFixture->GetShape();
	int vertexCount = originalPolygon->GetVertexCount();

	float determinant;
	int i;

	b2Vec2* sprite1Vertices = new b2Vec2[24];
	b2Vec2* sprite2Vertices = new b2Vec2[24];
	b2Vec2* sprite1VerticesSorted;
	b2Vec2* sprite2VerticesSorted;

	int sprite1VertexCount = 0;
	int sprite2VertexCount = 0;

	sprite1Vertices[sprite1VertexCount++] = sprite->getEntryPoint();
	sprite1Vertices[sprite1VertexCount++] = sprite->getExitPoint();
	sprite2Vertices[sprite2VertexCount++] = sprite->getEntryPoint();
	sprite2Vertices[sprite2VertexCount++] = sprite->getExitPoint();

	for(i = 0; i < vertexCount; i++){
		b2Vec2 point = originalPolygon->GetVertex(i);
		
		b2Vec2 diffFromEntryPoint = point - sprite->getEntryPoint();
		b2Vec2 diffFromExitPoint = point - sprite->getExitPoint();

		if((diffFromEntryPoint.x == 0 && diffFromEntryPoint.y == 0) || (diffFromExitPoint.x == 0 && diffFromExitPoint.y ==0)){
		}else{
			determinant = calculate_determinant_2x3(sprite->getEntryPoint().x, sprite->getEntryPoint().y,
				sprite->getExitPoint().y, sprite->getExitPoint().y, point.x, point.y);
			if(determinant > 0){
				sprite1Vertices[sprite1VertexCount++] = point;
			}else{
				sprite2Vertices[sprite2VertexCount++] = point;
			}
		}
	}

	sprite1VerticesSorted = this->arrangeVertices(sprite1Vertices, sprite1VertexCount);
	sprite2VerticesSorted = this->arrangeVertices(sprite2Vertices, sprite2VertexCount);

	bool sprite1VerticesAcceptable = this->areVerticesAcceptable(sprite1VerticesSorted, sprite1VertexCount);
	bool sprite2VerticesAcceptable = this->areVerticesAcceptable(sprite2VerticesSorted, sprite2VertexCount);

	if(sprite1VerticesAcceptable && sprite2VerticesAcceptable){
		_cuts++;
		_cutsLabel->setString(CCString::stringWithFormat("%d",_cuts)->getCString());

		b2Vec2 worldEntry = sprite->getBody()->GetWorldPoint(sprite->getEntryPoint());
		b2Vec2 worldExit = sprite->getBody()->GetWorldPoint(sprite->getExitPoint());
		float angle = ccpToAngle(ccpSub(ccp(worldExit.x, worldExit.y), ccp(worldEntry.x, worldEntry.y)));
		CCPoint vector1 = ccpForAngle(angle + 1.570796f);
		CCPoint vector2 = ccpForAngle(angle - 1.570796f);
		float midX = midpoint(worldEntry.x, worldExit.x);
		float midY = midpoint(worldEntry.y, worldExit.y);

		b2Body* body1 = this->createBodyX(sprite->getBody()->GetPosition(), sprite->getBody()->GetAngle(),
			sprite1VerticesSorted, sprite1VertexCount, originalFixture->GetDensity(), originalFixture->GetFriction(), originalFixture->GetRestitution());
		newSprite1 = PolygonSprite::spriteWithTexture(sprite->getTexture(), body1, false);
		newSprite1->getBody()->ApplyLinearImpulse(b2Vec2(2*body1->GetMass()*vector1.x,2*body1->GetMass()*vector1.y), b2Vec2(midX,midY));
		this->addChild(newSprite1, 1);

		b2Body* body2 = this->createBodyX(sprite->getBody()->GetPosition(), sprite->getBody()->GetAngle(),
			sprite2VerticesSorted, sprite2VertexCount, originalFixture->GetDensity(), originalFixture->GetFriction(), originalFixture->GetRestitution());
		newSprite2 = PolygonSprite::spriteWithTexture(sprite->getTexture(), body2, false);
		newSprite2->getBody()->ApplyLinearImpulse(b2Vec2(2*body2->GetMass()*vector2.x,2*body2->GetMass()*vector2.y), b2Vec2(midX,midY));
		this->addChild(newSprite2, 1);

		if(sprite->getOriginal()){
			b2Vec2 convertedWorldEntry = b2Vec2(worldEntry.x*PTM_RATIO,worldEntry.y*PTM_RATIO);
			b2Vec2 convertedWorldExit = b2Vec2(worldExit.x*PTM_RATIO,worldExit.y*PTM_RATIO);
			float midX = midpoint(convertedWorldEntry.x, convertedWorldExit.x);
			float midY = midpoint(convertedWorldEntry.y, convertedWorldExit.y);
			sprite->getSplurt()->setPosition(ccp(midX, midY));
			sprite->getSplurt()->resetSystem();

			sprite->setState(kStateIdle);
			sprite->deactivateCollisions();
			sprite->setPosition(ccp(-256, -256));
			sprite->setSliceEntered(false);
			sprite->setSliceExited(false);
			sprite->setEntryPoint(b2Vec2(0, 0));
			sprite->setExitPoint(b2Vec2(0, 0));

			if(sprite->getType() == kTypeBomb){
				SimpleAudioEngine::sharedEngine()->playEffect("explosion.mp3");
				this->subtractLife();
			}else{
				SimpleAudioEngine::sharedEngine()->playEffect("squash.mp3");
			}

		}else{
			flag = true;
			SimpleAudioEngine::sharedEngine()->playEffect("smallcut.mp3");
			//world->DestroyBody(sprite->getBody());
			//this->removeChild(sprite, true);
		}

	}else{		
		sprite->setSliceEntered(false);
		sprite->setSliceExited(false);
	}

	delete[] sprite1VerticesSorted;
	delete[] sprite2VerticesSorted;
	delete[] sprite1Vertices;
	delete[] sprite2Vertices;

	return flag;
}


b2Body* HelloWorld::createBodyX(b2Vec2 position, float rotation, b2Vec2* vertices, int32 count, float density, float friction, float restitution){
	
	b2BodyDef bodydef;
	bodydef.type = b2_dynamicBody;
	bodydef.position = position;
	bodydef.angle = rotation;
	b2Body* body = world->CreateBody(&bodydef);

	b2FixtureDef fixtureDef;
	fixtureDef.density = density;
	fixtureDef.friction = friction;
	fixtureDef.restitution = restitution;

	b2PolygonShape shape;
	shape.Set(vertices, count);
	fixtureDef.shape = &shape;	
	body->CreateFixture(&fixtureDef);

	return body;
}


b2Vec2* HelloWorld::arrangeVertices(b2Vec2* vertices, int count){
	float determinant;
	int iCounterClockWise = 1;
	int iClockWise = count - 1;
	int i;

	b2Vec2 referencePointA,referencePointB;
	b2Vec2 *sortedVertices = new b2Vec2[count];

	qsort(vertices, count, sizeof(b2Vec2),comparator);

	sortedVertices[0] = vertices[0];
	referencePointA = vertices[0];         
	referencePointB = vertices[count-1]; 
	
	for (i=1;i<count-1;i++)
	{
		determinant = calculate_determinant_2x3(referencePointA.x, referencePointA.y, 
			referencePointB.x, referencePointB.y, vertices[i].x, vertices[i].y);
		if (determinant<0){
			sortedVertices[iCounterClockWise++] = vertices[i];
		}else {
			sortedVertices[iClockWise--] = vertices[i];
		}
	}

	sortedVertices[iCounterClockWise] = vertices[count-1];
	return sortedVertices;
}

bool HelloWorld::areVerticesAcceptable(b2Vec2* vertices, int count){
	if(count < 3)
		return false;

	if(count > b2_maxPolygonVertices){
		return false;
	}

	int32 i;
	for(i = 0; i < count; i++){
		int32 i1 = i;
		int32 i2 = i + 1 < count ? i + 1 : 0;
		b2Vec2 edge = vertices[i2] - vertices[i1];
		if(edge.LengthSquared() <= b2_epsilon * b2_epsilon){
			return false;
		}
	}

	float32 area = 0.0f;
	b2Vec2 pRef(0.0f, 0.0f);
	for(i = 0; i < count; i++){
		b2Vec2 p1 = pRef;
		b2Vec2 p2 = vertices[i];
		b2Vec2 p3 = i + 1 < count ? vertices[i+1] : vertices[0];

		b2Vec2 e1 = p2 - p1;
		b2Vec2 e2 = p3 - p1;

		float32 D = b2Cross(e1, e2);

		float32 triangleArea = 0.5f * D;
		area += triangleArea;
	}

	if (area <= 0.0001f)
	{
		return false;
	}

	/*
	float determinant;
	float referenceDeterminant;
	b2Vec2 v1 = vertices[0] - vertices[count-1];
	b2Vec2 v2 = vertices[1] - vertices[0];
	referenceDeterminant = calculate_determinant_2x2(v1.x, v1.y, v2.x, v2.y);

	for (i=1; i<count-1; i++)
	{
		v1 = v2;
		v2 = vertices[i+1] - vertices[i];
		determinant = calculate_determinant_2x2(v1.x, v1.y, v2.x, v2.y);
		
		if (referenceDeterminant * determinant < 0.0f)
		{
			return false;
		}
	}
	v1 = v2;
	v2 = vertices[0]-vertices[count-1];
	determinant = calculate_determinant_2x2(v1.x, v1.y, v2.x, v2.y);
	if (referenceDeterminant * determinant < 0.0f)
	{
		return false;
	}*/

	// Ensure the polygon is convex and the interior
	// is to the left of each edge.
	for (int32 i = 0; i < count; ++i)
	{
		int32 i1 = i;
		int32 i2 = i + 1 < count ? i + 1 : 0;
		b2Vec2 edge = vertices[i2] - vertices[i1];

		for (int32 j = 0; j < count; ++j)
		{
			// Don't check vertices on the current edge.
			if (j == i1 || j == i2)
			{
				continue;
			}

			b2Vec2 r = vertices[j] - vertices[i1];

			// If this crashes, your polygon is non-convex, has colinear edges,
			// or the winding order is wrong.
			float32 s = b2Cross(edge, r);
			//b2Assert(s > 0.0f && "ERROR: Please ensure your polygon is convex and has a CCW winding order");
			if(s < 0.0f){
				return false;
			}
		}
	}


	return true;
}

void HelloWorld::checkAndSliceObjects(){
	double curTime = (double)millisecondNow();

	CCArray* _oldBodyToDel = CCArray::array();

	for(b2Body* b = world->GetBodyList(); b; b= b->GetNext()){
		if(b->GetUserData() != NULL){
			PolygonSprite* sprite = (PolygonSprite*)b->GetUserData();
			if(sprite->getSliceEntered() && curTime > sprite->getSliceEntryTime()){
				sprite->setSliceEntered(false);
			}else if(sprite->getSliceEntered() && sprite->getSliceExited()){
				bool flag = this->splitPolygonSprite(sprite);
				if(flag){
					_oldBodyToDel->addObject(sprite);
				}
			}
		}
	}

	for(unsigned int i = 0; i < _oldBodyToDel->count(); i++){
		PolygonSprite* sprite = (PolygonSprite*)_oldBodyToDel->objectAtIndex(i);
		world->DestroyBody(sprite->getBody());
		this->removeChild(sprite, true);
	}
	_oldBodyToDel->removeAllObjects();
}

void HelloWorld::clearSlices(){
	for(b2Body* b = world->GetBodyList(); b; b= b->GetNext()){
		if(b->GetUserData() != NULL){
			PolygonSprite* sprite = (PolygonSprite*)b->GetUserData();
			sprite->setSliceEntered(false);
			sprite->setSliceExited(false);
		}
	}
}

void HelloWorld::tossSprite(PolygonSprite* sprite){
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	CCPoint randomPosition = ccp(frandom_range(100.0f, winSize.width - 164.0f), -64.0f);
	float randomAngularVelocity = CCRANDOM_MINUS1_1();

	float xModifier = 50*(randomPosition.x - 100)/(winSize.width - 264);
	float min = -25.0 - xModifier;
	float max = 75.0 - xModifier;

	float randomXVelocity = frandom_range(min,max);
	float randomYVelocity = frandom_range(250, 300);

	sprite->setState(kStateTossed);
	sprite->setPosition(randomPosition);
	sprite->activateCollisions();
	sprite->getBody()->SetLinearVelocity(b2Vec2(randomXVelocity / PTM_RATIO, randomYVelocity / PTM_RATIO));
	sprite->getBody()->SetAngularVelocity(randomAngularVelocity);
}

void HelloWorld::spriteLoop(){
	double curTime = millisecondNow();

	if(curTime > _nextTossTime){
		CCObject* temp;

		int chance = rand() % 8;
		if(chance == 0){
			CCARRAY_FOREACH(_cache, temp){
				PolygonSprite* sprite = (PolygonSprite*)temp;
				if(sprite->getState() == kStateIdle && sprite->getType() == kTypeBomb){
					this->tossSprite(sprite);
					SimpleAudioEngine::sharedEngine()->playEffect("toss_bomb.mp3");
					break;
				}
			}
		}

		int random = random_range(0, 4);

		Type type = (Type)random;
		if(_currentTossType == kTossConsecutive && _queuedForToss > 0){
			SimpleAudioEngine::sharedEngine()->playEffect("toss_consecutive.mp3");
			CCARRAY_FOREACH(_cache, temp){
				PolygonSprite* sprite = (PolygonSprite*)temp;
				if(sprite->getState() == kStateIdle && sprite->getType() == type){
					this->tossSprite(sprite);
					_queuedForToss--;
					break;
				}
			}
		}else{
			_queuedForToss = random_range(3, 8);
			int tossType = random_range(0, 1);

			_currentTossType = (TossType)tossType;

			if(_currentTossType == kTossSimultaneous){
				SimpleAudioEngine::sharedEngine()->playEffect("toss_simultaneous.mp3");
				CCARRAY_FOREACH(_cache, temp){
					PolygonSprite* sprite = (PolygonSprite*)temp;
					if(sprite->getState() == kStateIdle && sprite->getType() == type){
						this->tossSprite(sprite);
						_queuedForToss --;

						random = random_range(0, 4);
						type = (Type)random;

						if (_queuedForToss == 0)
						{
							break;
						}
					}
				}
			}else if(_currentTossType == kTossConsecutive){
				SimpleAudioEngine::sharedEngine()->playEffect("toss_consecutive.mp3");
				CCARRAY_FOREACH(_cache, temp){
					PolygonSprite* sprite = (PolygonSprite*)temp;
					if(sprite->getState() == kStateIdle && sprite->getType() == type){
						this->tossSprite(sprite);
						_queuedForToss--;
						break;
					}
				}
			}
		}

		if(_queuedForToss == 0){
			_tossInterval = frandom_range(2, 3);
			_nextTossTime = curTime + _tossInterval * 1000.0f;
		}else{
			_tossInterval = frandom_range(0.3f, 0.8f);
			_nextTossTime = curTime + _tossInterval * 1000.0f;
		}
	}
}

void HelloWorld::cleanSprites(){
	CCObject* temp;
	CCARRAY_FOREACH(_cache, temp){
		PolygonSprite* sprite = (PolygonSprite*)temp;
		if(sprite->getState() == kStateTossed){
			CCPoint spritePosition = ccp(sprite->getBody()->GetPosition().x * PTM_RATIO, sprite->getBody()->GetPosition().y * PTM_RATIO);
			float yVelocity = sprite->getBody()->GetLinearVelocity().y;

			if(spritePosition.y < -64 && yVelocity < 0){
				if(sprite->getType() != kTypeBomb){
					this->subtractLife();
				}

				sprite->setState(kStateIdle);
				sprite->setSliceEntered(false);
				sprite->setSliceExited(false);
				sprite->setEntryPoint(b2Vec2(0.0f, 0.0f));
				sprite->setExitPoint(b2Vec2(0.0f, 0.0f));
				sprite->setPosition(ccp(-64, -64));
				sprite->getBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
				sprite->getBody()->SetAngularVelocity(0.0f);
				sprite->deactivateCollisions();
			}
		}
	}

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	CCArray* _oldBodyToDel = CCArray::array();
	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext()){
		if(! b->GetUserData()){
			PolygonSprite* sprite = (PolygonSprite*)b->GetUserData();
			CCPoint position = ccp(b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO);
			if(position.x < -64.0f || position.x > winSize.width + 64.0f || position.y < -64.0f){
				if(!sprite->getOriginal()){
					_oldBodyToDel->addObject(sprite);
				}
			}
		}
	}

	for(unsigned int i = 0; i < _oldBodyToDel->count(); i++){
		PolygonSprite* sprite = (PolygonSprite*)_oldBodyToDel->objectAtIndex(i);
		world->DestroyBody(sprite->getBody());
		this->removeChild(sprite, true);
	}
	_oldBodyToDel->removeAllObjects();
}

void HelloWorld::restart(CCObject* pSender){
	CCDirector::sharedDirector()->replaceScene(HelloWorld::scene());
}

void HelloWorld::endGame(){
	this->unscheduleUpdate();

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	CCMenuItemLabel* label = CCMenuItemLabel::itemWithLabel(CCLabelTTF::labelWithString("RESTART", "Helvetica Neue", 50), 
		this, menu_selector(HelloWorld::restart));
	CCMenu* menu = CCMenu::menuWithItems(label, NULL);
	menu->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f));
	this->addChild(menu, 4);
}

void HelloWorld::subtractLife(){
	SimpleAudioEngine::sharedEngine()->playEffect("lose_life.mp3");

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	_lives --;
	CCSprite* lostLife = CCSprite::spriteWithFile("x_filled.png");
	lostLife->setPosition(ccp(winSize.width - lostLife->getContentSize().width  * 0.5f - _lives * lostLife->getContentSize().width,
		winSize.height - lostLife->getContentSize().height * 0.5f));
	this->addChild(lostLife, 4);

	if(_lives <= 0){
		this->endGame();
	}
}
