#include <AI\Pathing\NavigationMeshGeneration.h>
#include <stdio.h>
#include <cmath>
#include <Physics\Physics.h>
#include <Engine\DebugWindow.h>
#include <Entity\StaticObject.h>
#include <assert.h>
#include <Physics\FuncContactResult.h>

using namespace Logic;

#define EPSILON 0.001f
#define toSimple(vec) {vec.x(), vec.y(), vec.z()}

const int NavigationMeshGeneration::AI_UID = 1061923, NavigationMeshGeneration::NO_ID = -5;
const float NavigationMeshGeneration::SEED_CUBES = 100.f, NavigationMeshGeneration::PRECISION_BASE = 0.05f;
int NavigationMeshGeneration::COUNTER = 0;
const btVector3 NavigationMeshGeneration::unitDimension = { 0.2f, 0.2f, 0.2f }; // i know it is not 1, todo

NavigationMeshGeneration::NavigationMeshGeneration()
{
    precision = PRECISION_BASE;
    maxLength = 250.f;
    baseY = 0.5f;

    registerSetCommands();

    growth[X_PLUS] = { { precision, 0.f, 0.f }, { precision, 0.f, 0.f } }; // X +
    growth[Z_MINUS] = { { 0.f, 0.f, -precision },{ 0.f, 0.f, precision } }; // Z-
    growth[X_MINUS] = { { -precision , 0.f, 0.f },{ precision, 0.f, 0.f } }; // X-
    growth[Z_PLUS] = { { 0.f, 0.f, precision },{ 0.f, 0.f, precision } }; // Z+

    growthNormals[X_PLUS] = { 1.f, 0.f, 0.f };
    growthNormals[Z_MINUS] = { 0.f, 0.f, -1.f };
    growthNormals[X_MINUS] = { -1.f, 0.f, 0.f };
    growthNormals[Z_PLUS] = { 0.f, 0.f, 1.f };
}

NavigationMeshGeneration::~NavigationMeshGeneration()
{
}

void NavigationMeshGeneration::registerSetCommands()
{
    DebugWindow::getInstance()->registerCommand("AI_NAV_SET_PRECISION", [&](std::vector<std::string> &para) -> std::string {
        try {
            precision = std::stof(para.at(0));
            return "Presicion set to " + para.at(0);
        }
        catch (std::exception ex) {
            return "Set presicion, not GET presicion.";
        }
    });
    DebugWindow::getInstance()->registerCommand("AI_NAV_SET_MAX_LEN", [&](std::vector<std::string> &para) -> std::string {
        try {
            maxLength = std::stof(para.at(0));
            return "Max length set to " + para.at(0);
        }
        catch (std::exception ex) {
            return "When life gives you lemon, (parameter wrong)";
        }
    });
    DebugWindow::getInstance()->registerCommand("AI_NAV_SET_BASE_Y", [&](std::vector<std::string> &para) -> std::string {
        try {
            baseY = std::stof(para.at(0));
            return "Base Y set to " + para.at(0);
        }
        catch (std::exception ex) {
            return "Make lemonade. (parameter wrong)";
        }
    });
}

void NavigationMeshGeneration::registerGenerationCommand(
    NavigationMesh &nav, Physics &physics)
{
    DebugWindow::getInstance()->registerCommand("AI_NAV_LOAD_MESH_OLD",
        [&](std::vector<std::string>) -> std::string {
        nav.clear();
        generateNavMeshOld(nav, {}, {});
        return "Generated Nav Mesh (!OLD, STATIC & DEPREACETED!)";
    });
    DebugWindow::getInstance()->registerCommand("AI_NAV_LOAD_MESH",
        [&](std::vector<std::string>) -> std::string {
        nav.clear();
        generateNavigationMesh(nav, physics);
        return "Generated Nav Mesh (NOT FULLY IMPLEMENTED!!!!!)";
    });
}

// This will be removed in the future ;D 
void NavigationMeshGeneration::generateNavMeshOld(NavigationMesh &nav,
    std::vector<Triangle> terrain,
    std::vector<NavStaticObject> objects) const
{
#define T 50.f // size
#define Y 0.f // node y position
    Triangle t;
    std::vector<Triangle> moveableTerrain;

    for (int z = -6; z < 6; z++)
    {
        for (int x = -6; x < 6; x++)
        {
            t.vertices[0] = DirectX::SimpleMath::Vector3(T * x, Y, T * z);
            t.vertices[1] = DirectX::SimpleMath::Vector3(T * (x + 1), Y, T * (z + 1));
            t.vertices[2] = DirectX::SimpleMath::Vector3(T * x, Y, T * (z + 1));
            terrain.push_back(t);

            t.vertices[1] = DirectX::SimpleMath::Vector3(T * (x + 1), Y, T * z);
            t.vertices[2] = DirectX::SimpleMath::Vector3(T * (x + 1), Y, T * (z + 1));
            terrain.push_back(t);
        }
    }

	DirectX::SimpleMath::Vector3 up = { 0, 1, 0 };
	float normalDotMin = 0.6f;

	// part 1 of the generation, make terrain into a "walkable" terrain map
	for (auto &triangle : terrain)
	{
		triangle.normal = getNormal(triangle);
		triangle.normal.Normalize();
		if (abs(triangle.normal.Dot(up)) > normalDotMin)
			moveableTerrain.push_back(triangle); // if triangle is too steep, enemy wont be able to go up it
	}

	// part 2, use the objects break the moveable terrain
	

	// part ??? of the generation, add it to the nav mesh because it is done :)
	for (auto &triangle : moveableTerrain) 
	{
		nav.addTriangle(
			{
				0,
				{
					triangle.vertices[0],
					triangle.vertices[1],
					triangle.vertices[2]
				}
		});
	}
    nav.createNodesFromTriangles();
    nav.generateEdges();
}

void NavigationMeshGeneration::generateNavigationMesh(NavigationMesh &nav,
    Physics &physics)
{
    regions.clear();
    regions.reserve(1500); // THIS IS A TEMPORARY SOLUTION TO PREVENT MEMORY FOK UPS; MAKE A REAL SOLUTION

    printf("Seeding area..");
    seedArea({ -125.f, 0.6f, -135.f }, { 250.f, 0.5f, 300.f }, SEED_CUBES, physics);
    printf("Seeding finished!\n");

    btCollisionObject *obj;
    StaticObject *staticObj;
    float distance;

    // first cube
    printf("Buckleup buckero this will take a while! Generating Navigation Mesh...\n");
   // testFunc(physics);
    for (size_t index = 0; index < regions.size(); index++) // elements is added inside vec
    {
        auto &region = regions[index];
        printf("Loading.. %f %%\n", static_cast<float> (region.userIndex) / COUNTER * 100.f);

        if (isInCollisionArea(region, physics, region.buddyIndex, region.userIndex))
        {
            region.remove = true;
            continue;
        }

        for (int side = 0; side < SIDES && !region.collided[side] && !region.done; side++)
        {
            distance = 0.f;
            while (!region.collided[side] && distance < maxLength && !region.done)
            {
                growRegion(region, growth[side]);

                if (region.body) removeRigidBody(region.body);
                region.body = physics.createBody(region.cube, 0.f);
                physics.removeRigidBody(region.body);
                
                handlePhysicsCollisionTest(region, physics, side);
                handleRegionCollisionTest(region, physics, side);

                distance += precision * 2;
            }
        }

        region.done = true;
        region.body->setUserIndex2(region.userIndex);
        region.body->setUserIndex(AI_UID);
    }

    quadMeshToTriangleMesh(nav, physics);
    printf("Finished! :D");
}

DirectX::SimpleMath::Vector3 NavigationMeshGeneration::getNormal(
    Triangle const &triangle, VertexOrder vertexOrder) const {
	if (vertexOrder == CLOCKWISE)
		return (triangle.vertices[1] - triangle.vertices[0])
            .Cross(triangle.vertices[2] - triangle.vertices[0]);
    else
        return (triangle.vertices[0] - triangle.vertices[1])
            .Cross(triangle.vertices[0] - triangle.vertices[2]);
}

std::pair<NavigationMeshGeneration::Triangle, NavigationMeshGeneration::Triangle>
    NavigationMeshGeneration::toTriangle(Cube &cube)
{
    float y = 0;
    Triangle tri1, tri2;
    cube.getDimensionsRef().setY(y);

    tri1.vertices[0] = DirectX::SimpleMath::Vector3(cube.getPos() - cube.getDimensions());
    tri1.vertices[1] = DirectX::SimpleMath::Vector3(cube.getPos() + btVector3{ cube.getDimensions().x(), y, -cube.getDimensions().z() });
    tri1.vertices[2] = DirectX::SimpleMath::Vector3(cube.getPos() + btVector3{ -cube.getDimensions().x(), y, cube.getDimensions().z() });

    tri2.vertices[0] = DirectX::SimpleMath::Vector3(cube.getPos() + btVector3{ cube.getDimensions().x(), y, -cube.getDimensions().z() });
    tri2.vertices[1] = DirectX::SimpleMath::Vector3(cube.getPos() + cube.getDimensions());
    tri2.vertices[2] = DirectX::SimpleMath::Vector3(cube.getPos() + btVector3{ -cube.getDimensions().x(), y, cube.getDimensions().z() });
     
    return std::pair<Triangle, Triangle>(tri1, tri2);
}

std::pair<Cube, Cube> NavigationMeshGeneration::cutCube(btVector3 const &cutPoint, btVector3 const &planeNormal, Cube const &cube)
{
    std::pair<Cube, Cube> cubes(cube, cube);
    Cube &f = cubes.first, &s = cubes.second;

    btVector3 end = cube.getPos() + cube.getDimensions();
    btVector3 toPos = cutPoint - end;
    btVector3 temp;

    float distToPlane = abs(abs(toPos.dot(planeNormal)) / planeNormal.length2()) * 0.5f;

    if (planeNormal.x() != 0)
    {
        f.getDimensionsRef().setX(distToPlane);
        s.getDimensionsRef().setX(s.getDimensionsRef().x() - distToPlane);

        temp = f.getPos();
        temp.setX(end.x() - distToPlane);
        f.setPos(temp);

        temp = s.getPos();
        temp.setX(end.x() - f.getDimensionsRef().x() * 2 - s.getDimensionsRef().x());
        s.setPos(temp);
    }
    else
    {
        f.getDimensionsRef().setZ(distToPlane);
        s.getDimensionsRef().setZ(s.getDimensionsRef().z() - distToPlane);

        temp = f.getPos();
        temp.setZ(end.z() - distToPlane);
        f.setPos(temp);

        temp = s.getPos();
        temp.setZ(end.z() - f.getDimensionsRef().z() * 2 - s.getDimensionsRef().z());
        s.setPos(temp);
    }

    return cubes;
}

void NavigationMeshGeneration::handlePhysicsCollisionTest(NavMeshCube &region, Physics &physics, int side)
{
    FunContactResult res(
        [&](btBroadphaseProxy* proxy) -> bool {
        if (!proxy->isConvex2d(TRIANGLE_MESH_SHAPE_PROXYTYPE) || !proxy->isConvex(TRIANGLE_MESH_SHAPE_PROXYTYPE))
        {
            printf("WARNING: NON CONVEX OBJECT!\nNAV MESH GENERATION UNDEFINED BEHAVIOR!\n");
            return false;
        }
        return true;
    },
        [&](btManifoldPoint& cp,
            const btCollisionObjectWrapper* colObj0, int partId0, int index0,
            const btCollisionObjectWrapper* colObj1, int partId1, int index1) -> btScalar
    {
        if (abs(cp.getDistance()) > precision) return 0;
        if (StaticObject* staticObj = dynamic_cast<StaticObject*> (reinterpret_cast<PhysicsObject*> (colObj1->getCollisionObject()->getUserPointer())))
        {
            if (!(staticObj->getNavFlags() & StaticObject::NavigationMeshFlags::CULL))
            {
                if (btBoxShape* bs = dynamic_cast<btBoxShape*>(staticObj->getRigidBody()->getCollisionShape())) // only support box shapes at the moment (other shapes can be "converted" to boxes)
                {
                    cp.m_localPointA += region.body->getWorldTransform().getOrigin();
                    CollisionReturn ret = handleCollision(cp.m_localPointA, region, staticObj, growth[side], growthNormals[side], bs);

                    switch (ret)
                    {
                    case ON_VERTEX:
                        split(region, physics, cp.m_localPointA, growthNormals[(side + 1) % SIDES]); // clockwise rotation
                        break;
                    }
                    if (ret != PROBLEMS_MY_DUDES) region.collided[side] = true;
                }
            }
        }
        return 0;
    });

    for (int i = 0; i < physics.getNumCollisionObjects() && !region.collided[side] && !region.done; i++)
        physics.contactPairTest(region.body, physics.getCollisionObjectArray()[i], res);
}

void NavigationMeshGeneration::handleRegionCollisionTest(NavMeshCube &region, Physics &physics, int side)
{
    FunContactResult resRegions(
        [&](btBroadphaseProxy* proxy) -> bool {
        return true;
    },
        [&](btManifoldPoint& cp,
            const btCollisionObjectWrapper* colObj0, int partId0, int index0,
            const btCollisionObjectWrapper* colObj1, int partId1, int index1) -> btScalar
    {
        if (abs(cp.getDistance()) < precision)
        {
            const btCollisionObject *obj = colObj1->getCollisionObject();

            region.collided[side] = true;
            region.addCollision(side, obj->getUserIndex2());
        }

        return 0;
    });

    for (auto &other : regions)
        if (&other != &region && other.userIndex != region.buddyIndex)
            physics.contactPairTest(region.body, other.body, resRegions);

    if (region.collided[side]) // can be close to multiple regions
        shrinkRegion(region, growth[side]);
}

void NavigationMeshGeneration::shrinkRegion(NavMeshCube &region, Growth const &growth)
{
    region.cube.setDimensions(region.cube.getDimensions() - growth.dimensionChange);
    region.cube.setPos(region.cube.getPos() - growth.positionChange);
}

btVector3 NavigationMeshGeneration::getDimension(NavMeshCube &region, int side) const
{
    switch (side) // use normals or something instead?
    {
        case X_PLUS:
            return { region.cube.getDimensionsRef().x(), 0.f, 0.f };
        case X_MINUS:
            return { -region.cube.getDimensionsRef().x(), 0.f, 0.f };
        case Z_PLUS:
            return { 0.f, 0.f, region.cube.getDimensionsRef().z() };
        case Z_MINUS:
            return { 0.f, 0.f, -region.cube.getDimensionsRef().z() };
    }
    return { 0.f, 0.f, 0.f };
}

int NavigationMeshGeneration::getRegion(int id) const
{
    int ret = -1;
    for (int i = 0; i < regions.size() && ret == -1; i++)
        if (regions[i].userIndex == id)
            ret = i;
    return ret;
}

void NavigationMeshGeneration::quadMeshToTriangleMesh(NavigationMesh &nav, Physics &physics)
{
    int index, userId;
    btVector3 top, bot, test;
    bool cutBot;

    for (size_t t = 0; t < regions.size(); t++) // remove bad seeds
        if (regions[t].remove)
        {
            std::swap(regions[t], regions[regions.size() - 1]);
            regions.pop_back();
            t--;
        }
    
    /*
    for (auto &region : regions) // two loops, rip
    {
        for (int side = 0; side < SIDES; side++)  // this is just awful, should optimize
        { 
            for (int userId : region.collidedWithIndex[side])
            {
                index = getRegion(userId);
                if (index != -1)
                {
                    NavMeshCube &other = regions[index];
                    if (!other.remove)
                    {
                        top = region.cube.getPos() + getDimension(region, (side + 1) % SIDES);
                        bot = region.cube.getPos() - getDimension(region, (side + 1) % SIDES);
                        cutBot = bot > regions[index].cube.getPos() + regions[index].cube.getDimensionsRef();

                        if (top < regions[index].cube.getPos() + regions[index].cube.getDimensionsRef())
                        {
                            physics.createBody(Sphere(top, { 0.f, 0.f, 0.f }, 1.f), 0);
                            split(regions[index], physics, top, growthNormals[static_cast<int> (abs(side - 1)) % SIDES]);
                        }
                        if (cutBot)
                        {
                            physics.createBody(Sphere(bot, { 0.f, 0.f, 0.f }, 1.f), 0);
                            split(regions[index], physics, bot, growthNormals[static_cast<int> (abs(side + 1)) % SIDES]);
                        }
                    }
                }
            }
        }
    } */

    for (auto &region : regions) // add remaining to navigation mesh
    {
        Cube &cube = region.cube;
        std::pair<Triangle, Triangle> triPair = toTriangle(cube);

        if (region.body)
            removeRigidBody(region.body);

        nav.addTriangle(toNavTriangle(triPair.first));
        nav.addTriangle(toNavTriangle(triPair.second));
    }

    nav.createNodesFromTriangles();
    
    int regionIndex, otherIndex;
    for (size_t t = 0; t < regions.size(); t++) // create nodes beetwen them
    {
        auto &region = regions[t];
        regionIndex = static_cast<int> (t);
        nav.addDoubleEdge(t * 2, t * 2 + 1, { 0,0,0 });

        for (int side = 0; side < SIDES; side++)
            for (int id : region.collidedWithIndex[side])
                if ((otherIndex = getRegion(id)) > -1)
                    createEdgeBeetwen(nav, regionIndex, otherIndex, static_cast<GrowthType> (side));
    }  
}

void NavigationMeshGeneration::growRegion(NavMeshCube &region, Growth const &growth)
{
    region.cube.setDimensions(region.cube.getDimensions() + growth.dimensionChange);
    region.cube.setPos(region.cube.getPos() + growth.positionChange);
}

NavigationMesh::Triangle NavigationMeshGeneration::toNavTriangle(Triangle const & tri)
{
    return { 0, tri.vertices[0], tri.vertices[1], tri.vertices[2] };
}

NavigationMeshGeneration::CollisionReturn NavigationMeshGeneration::handleCollision(btVector3 collisionPoint,
    NavMeshCube &cube, StaticObject *obj, Growth const &growth, btVector3 growthNormal, btBoxShape *shape)
{
    shrinkRegion(cube, growth);

    DirectX::SimpleMath::Quaternion rot = obj->getRotation();
    rot.w = 0; // this is stupid

    if (rot.LengthSquared() < EPSILON) // base case
    {
        return ON_AXIS;
    }
    
    btVector3 vertex;
    bool found = false;

    for (int i = 0; i < shape->getNumVertices() && !found; i++)
    {
        shape->getVertex(i, vertex);
        if (shape->isInside(vertex, btScalar(0.05f)))
            found = true;
    }

    if (found) // case b, on vertex
        return ON_VERTEX;
    else // case c, living on the edge. (Collision on edge)
        return ON_EDGE;
}

void NavigationMeshGeneration::split(NavMeshCube &cube, Physics &physics, btVector3 const &cubeColPoint,
    btVector3 const &splitPlaneNormal)
{
    NavMeshCube cube1(cube);
    std::pair<Cube, Cube> btCubes = cutCube(cubeColPoint, splitPlaneNormal, cube.cube);

    cube1.cube = btCubes.first;
    cube.cube = btCubes.second;

    cube1.loadIndex(); // cube1 get a new index, while the other remains the "same"

    cube1.buddyIndex = cube.userIndex;
    cube.buddyIndex = cube1.userIndex;

    cube1.body = physics.createBody(cube1.cube, 0.f);
    cube1.body->setUserIndex(AI_UID);
    cube1.body->setUserIndex2(cube1.userIndex);
    addSplitIndices(cube.userIndex, cube.buddyIndex);
    
    regions.push_back(cube1);
}

void NavigationMeshGeneration::removeRigidBody(btRigidBody *&body)
{
    delete body->getMotionState();
    delete body->getCollisionShape();
    delete body;

    body = nullptr;
}

void NavigationMeshGeneration::addSplitIndices(int firstIndex, int secondIndex)
{
    for (auto &region : regions) // this is awful
        if (region.hasCollision(firstIndex))
        {
            int index = region.hasCollision(firstIndex);
            if (index > -1)
                region.addCollision(index, secondIndex);
        }
}

bool NavigationMeshGeneration::isInCollisionArea(NavMeshCube &cube, Physics &physics, int filterId0, int filterId1)
{
    bool collision = false;
    btCollisionObject *obj;

    btRigidBody *temp = physics.createBody(cube.cube, 0);
    physics.removeRigidBody(temp);
    for (int i = 0; i < physics.getNumCollisionObjects() && !collision; i++)
    {
        obj = physics.getCollisionObjectArray()[i];

        FunContactResult res([&](btBroadphaseProxy* proxy) -> bool {
            return true;
        },
            [&](btManifoldPoint& cp,
                const btCollisionObjectWrapper* colObj0, int partId0, int index0,
                const btCollisionObjectWrapper* colObj1, int partId1, int index1) -> btScalar
        {
            if (cp.getDistance() > precision) return 0;

            if (StaticObject *staticObj = dynamic_cast<StaticObject*> (reinterpret_cast<PhysicsObject*> (obj->getUserPointer())))
            {
                if (!(staticObj->getNavFlags() & StaticObject::NavigationMeshFlags::CULL))
                    if (btBoxShape* bs = dynamic_cast<btBoxShape*>(staticObj->getRigidBody()->getCollisionShape())) // only support box shapes at the moment (other shapes can be "converted" to boxes)
                        collision = true;
            }
            else if (obj->getUserIndex() == AI_UID && obj->getUserIndex2() != filterId0 && obj->getUserIndex2() != filterId1)
            {
                collision = true;
            }
            return 0;
        });
        physics.contactPairTest(temp, obj, res);
    }
    removeRigidBody(temp);
    return collision;
}

void NavigationMeshGeneration::seedArea(btVector3 position, btVector3 fullDimension,
    float part, Physics &physics)
{
    float startZ = position.z();
    btVector3 piece = fullDimension / part;

    for (float partX = 0.f; partX < fullDimension.x(); partX += piece.x())
    {
        position.setZ(startZ);
        position.setX(position.x() + piece.x());
        for (float partZ = 0.f; partZ < fullDimension.z(); partZ += piece.z())
        {
            position.setZ(position.z() + piece.z());
            NavMeshCube cube(Cube(position, { 0.f, 0.f, 0.f }, piece));
            if (!isInCollisionArea(cube, physics))
            {
                regions.push_back(cube);
            }
        }
    }
}

void NavigationMeshGeneration::createEdgeBeetwen(NavigationMesh &nav, int r1, int r2, GrowthType side)
{
    NavMeshCube &reg1 = regions[r1];
    NavMeshCube &reg2 = regions[r2];
    DirectX::SimpleMath::Vector3 con;
    btVector3 vec1 = reg1.cube.getPos() + getDimension(reg1, side),
              vec2 = reg2.cube.getPos() + getDimension(reg2, side + 2 % SIDES);

    btVector3 half1 = getDimension(reg1, side + 1 % SIDES),
              half2 = getDimension(reg2, side + 3 % SIDES);

    btVector3 to = vec2 - vec1;
    float dot = to.dot(half1);

    if (side == X_MINUS || side == Z_MINUS)
    {
        if (std::abs(dot) <= 1.f)
            nav.addDoubleEdge(r1 * 2, r2 * 2, toSimple(vec2));
        else
        {
            if (dot > 0.f)
                nav.addDoubleEdge(r1 * 2, r2 * 2, toSimple((vec1 + half1 + half2 * btScalar(0.5f))));
            else
                nav.addDoubleEdge(r1 * 2, r2 * 2, toSimple((vec1 + half1 - half2 * btScalar(0.5f))));
        }
    }
    else
    {
        if (std::abs(dot) <= 1.f)
            nav.addDoubleEdge(r1 * 2 + 1, r2 * 2 + 1, toSimple(vec2));
        else
        {
            if (dot > 0.f)
                nav.addDoubleEdge(r1 * 2 + 1, r2 * 2 + 1, toSimple((vec1 + half1 + half2 * btScalar(0.5f))));
            else
                nav.addDoubleEdge(r1 * 2 + 1, r2 * 2 + 1, toSimple((vec1 + half1 - half2 * btScalar(0.5f))));
        }
    }
}

std::pair<bool, btVector3> NavigationMeshGeneration::rayTestCollisionPoint(StaticObject *obj, btRigidBody *reg, Physics &physics, btVector3 &normalIncrease, float maxDistance)
{
    bool collision;
    btVector3 point;
    btVector3 endPoint = -1.f * (normalIncrease * maxDistance);
    btRigidBody *body = obj->getRigidBody();

    // THIS IS A TESTING SOLUTION FOR THE MOMENT, CUSTOM
    btCollisionWorld::ClosestRayResultCallback res(body->getWorldTransform().getOrigin(), endPoint);

    return std::pair<bool, btVector3>(collision, point);
}

void NavigationMeshGeneration::testFunc(Physics &physics)
{
    regions.push_back(NavMeshCube(Cube({ 0, 0, 0 }, { 0, 0, 0 }, { 25, 0.2f, 25 })));
    regions.push_back(NavMeshCube(Cube({ 30, 0, 0 }, { 0, 0, 0 }, { 5, 0.2f, 5 })));
    regions.push_back(NavMeshCube(Cube({ -30, 0, 10 }, { 0, 0, 0 }, { 5, 0.2f, 5 })));

    regions[1].addCollision(X_PLUS, regions[0].userIndex);
    regions[2].addCollision(X_MINUS, regions[0].userIndex);
}