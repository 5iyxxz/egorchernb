#include "..\e2dnode.h"
#include "..\e2dmanager.h"
#include "..\e2daction.h"
#include "..\e2dcollider.h"
#include <algorithm>

// 默认中心点位置
static float s_fDefaultPiovtX = 0;
static float s_fDefaultPiovtY = 0;
static bool s_fDefaultColliderEnabled = true;

e2d::Node::Node()
	: _nOrder(0)
	, _fPosX(0)
	, _fPosY(0)
	, _fWidth(0)
	, _fHeight(0)
	, _fScaleX(1.0f)
	, _fScaleY(1.0f)
	, _fRotation(0)
	, _fSkewAngleX(0)
	, _fSkewAngleY(0)
	, _fDisplayOpacity(1.0f)
	, _fRealOpacity(1.0f)
	, _fPivotX(s_fDefaultPiovtX)
	, _fPivotY(s_fDefaultPiovtY)
	, _MatriInitial(D2D1::Matrix3x2F::Identity())
	, _MatriFinal(D2D1::Matrix3x2F::Identity())
	, _bVisiable(true)
	, _pCollider(nullptr)
	, _pParent(nullptr)
	, _pParentScene(nullptr)
	, _nHashName(0)
	, _bSortChildrenNeeded(false)
	, _bTransformNeeded(false)
	, _bAutoUpdate(true)
	, _bPositionFixed(false)
{
	if (s_fDefaultColliderEnabled)
	{
		auto rect = new ColliderRect(this);
		this->setCollider(rect);
	}
}

e2d::Node::~Node()
{
}

void e2d::Node::_update()
{
	if (_bTransformNeeded)
	{
		_updateTransform();
	}

	if (!_vChildren.empty())
	{
		if (_bSortChildrenNeeded)
		{
			// 子节点排序
			auto sortFunc = [](Node * n1, Node * n2) {
				return n1->getOrder() < n2->getOrder();
			};

			std::sort(
				std::begin(_vChildren),
				std::end(_vChildren),
				sortFunc
			);

			_bSortChildrenNeeded = false;
		}

		// 遍历子节点
		size_t size = _vChildren.size();
		size_t i;
		for (i = 0; i < size; i++)
		{
			auto child = _vChildren[i];
			// 访问 Order 小于零的节点
			if (child->getOrder() < 0)
			{
				child->_update();
			}
			else
			{
				break;
			}
		}

		if (_bAutoUpdate)
		{
			if (!Game::isPaused())
			{
				this->onUpdate();
			}
			this->onFixedUpdate();
		}

		// 访问剩余节点
		for (; i < size; i++)
			_vChildren[i]->_update();
	}
	else
	{
		if (_bAutoUpdate)
		{
			if (!Game::isPaused())
			{
				this->onUpdate();
			}
			this->onFixedUpdate();
		}
	}
}

void e2d::Node::_render()
{
	if (!_bVisiable)
	{
		return;
	}

	if (!_vChildren.empty())
	{
		size_t size = _vChildren.size();
		size_t i;
		for (i = 0; i < size; i++)
		{
			auto child = _vChildren[i];
			// 访问 Order 小于零的节点
			if (child->getOrder() < 0)
			{
				child->_render();
			}
			else
			{
				break;
			}
		}

		// 转换渲染器的二维矩阵
		Renderer::getRenderTarget()->SetTransform(_MatriFinal);
		// 渲染自身
		this->onRender();

		// 访问剩余节点
		for (; i < size; i++)
			_vChildren[i]->_render();
	}
	else
	{
		// 转换渲染器的二维矩阵
		Renderer::getRenderTarget()->SetTransform(_MatriFinal);
		// 渲染自身
		this->onRender();
	}
}

void e2d::Node::_drawCollider()
{
	// 绘制自身的几何碰撞体
	if (_pCollider && _pCollider->_bIsVisiable)
	{
		_pCollider->_render();
	}

	// 绘制所有子节点的几何碰撞体
	for (auto child : _vChildren)
	{
		child->_drawCollider();
	}
}

void e2d::Node::_updateSelfTransform()
{
	// 计算中心点坐标
	D2D1_POINT_2F pivot = { _fWidth * _fPivotX, _fHeight * _fPivotY };
	// 变换 Initial 矩阵，子节点将根据这个矩阵进行变换
	_MatriInitial = D2D1::Matrix3x2F::Scale(
		_fScaleX,
		_fScaleY,
		pivot
	) * D2D1::Matrix3x2F::Skew(
		_fSkewAngleX,
		_fSkewAngleY,
		pivot
	) * D2D1::Matrix3x2F::Rotation(
		_fRotation,
		pivot
	) * D2D1::Matrix3x2F::Translation(
		_fPosX,
		_fPosY
	);
	// 根据自身中心点变换 Final 矩阵
	_MatriFinal = _MatriInitial * D2D1::Matrix3x2F::Translation(-pivot.x, -pivot.y);
	// 和父节点矩阵相乘
	if (!_bPositionFixed && _pParent)
	{
		_MatriInitial = _MatriInitial * _pParent->_MatriInitial;
		_MatriFinal = _MatriFinal * _pParent->_MatriInitial;
	}
}

void e2d::Node::_updateTransform()
{
	// 计算自身的转换矩阵
	_updateSelfTransform();
	// 绑定于自身的碰撞体也进行相应转换
	if (_pCollider)
	{
		_pCollider->_transform();
	}
	// 标志已执行过变换
	_bTransformNeeded = false;
	// 遍历子节点下的所有节点
	for (auto child : this->_vChildren)
	{
		child->_updateTransform();
	}
}

void e2d::Node::_updateOpacity()
{
	if (_pParent)
	{
		_fDisplayOpacity = _fRealOpacity * _pParent->_fDisplayOpacity;
	}
	for (auto child : _vChildren)
	{
		child->_updateOpacity();
	}
}

bool e2d::Node::isVisiable() const
{
	return _bVisiable;
}

e2d::String e2d::Node::getName() const
{
	return _name;
}

unsigned int e2d::Node::getHashName() const
{
	return _nHashName;
}

double e2d::Node::getPosX() const
{
	return _fPosX;
}

double e2d::Node::getPosY() const
{
	return _fPosY;
}

e2d::Point e2d::Node::getPos() const
{
	return Point(_fPosX, _fPosY);
}

double e2d::Node::getWidth() const
{
	return _fWidth * _fScaleX;
}

double e2d::Node::getHeight() const
{
	return _fHeight * _fScaleY;
}

double e2d::Node::getRealWidth() const
{
	return _fWidth;
}

double e2d::Node::getRealHeight() const
{
	return _fHeight;
}

e2d::Size e2d::Node::getRealSize() const
{
	return Size(_fWidth, _fHeight);
}

double e2d::Node::getPivotX() const
{
	return _fPivotX;
}

double e2d::Node::getPivotY() const
{
	return _fPivotY;
}

e2d::Size e2d::Node::getSize() const
{
	return Size(getWidth(), getHeight());
}

double e2d::Node::getScaleX() const
{
	return _fScaleX;
}

double e2d::Node::getScaleY() const
{
	return _fScaleY;
}

double e2d::Node::getSkewX() const
{
	return _fSkewAngleX;
}

double e2d::Node::getSkewY() const
{
	return _fSkewAngleY;
}

double e2d::Node::getRotation() const
{
	return _fRotation;
}

double e2d::Node::getOpacity() const
{
	return _fRealOpacity;
}

e2d::NodeProperty e2d::Node::getProperty() const
{
	NodeProperty prop;
	prop.visable = _bVisiable;
	prop.posX = _fPosX;
	prop.posY = _fPosY;
	prop.width = _fWidth;
	prop.height = _fHeight;
	prop.opacity = _fRealOpacity;
	prop.pivotX = _fPivotX;
	prop.pivotY = _fPivotY;
	prop.scaleX = _fScaleX;
	prop.scaleY = _fScaleY;
	prop.rotation = _fRotation;
	prop.skewAngleX = _fSkewAngleX;
	prop.skewAngleY = _fSkewAngleY;
	return prop;
}

e2d::Collider * e2d::Node::getCollider() const
{
	return _pCollider;
}

int e2d::Node::getOrder() const
{
	return _nOrder;
}

void e2d::Node::setOrder(int order)
{
	_nOrder = order;
}

void e2d::Node::setPosX(double x)
{
	this->setPos(x, _fPosY);
}

void e2d::Node::setPosY(double y)
{
	this->setPos(_fPosX, y);
}

void e2d::Node::setPos(const Point & p)
{
	this->setPos(p.x, p.y);
}

void e2d::Node::setPos(double x, double y)
{
	if (_fPosX == x && _fPosY == y)
		return;

	_fPosX = static_cast<float>(x);
	_fPosY = static_cast<float>(y);
	_bTransformNeeded = true;
}

void e2d::Node::setPosFixed(bool fixed)
{
	if (_bPositionFixed == fixed)
		return;

	_bPositionFixed = fixed;
	_bTransformNeeded = true;
}

void e2d::Node::movePosX(double x)
{
	this->movePos(x, 0);
}

void e2d::Node::movePosY(double y)
{
	this->movePos(0, y);
}

void e2d::Node::movePos(double x, double y)
{
	this->setPos(_fPosX + x, _fPosY + y);
}

void e2d::Node::movePos(const Vector & v)
{
	this->movePos(v.x, v.y);
}

void e2d::Node::setScaleX(double scaleX)
{
	this->setScale(scaleX, _fScaleY);
}

void e2d::Node::setScaleY(double scaleY)
{
	this->setScale(_fScaleX, scaleY);
}

void e2d::Node::setScale(double scale)
{
	this->setScale(scale, scale);
}

void e2d::Node::setScale(double scaleX, double scaleY)
{
	if (_fScaleX == scaleX && _fScaleY == scaleY)
		return;

	_fScaleX = static_cast<float>(scaleX);
	_fScaleY = static_cast<float>(scaleY);
	_bTransformNeeded = true;
}

void e2d::Node::setSkewX(double angleX)
{
	this->setSkew(angleX, _fSkewAngleY);
}

void e2d::Node::setSkewY(double angleY)
{
	this->setSkew(_fSkewAngleX, angleY);
}

void e2d::Node::setSkew(double angleX, double angleY)
{
	if (_fSkewAngleX == angleX && _fSkewAngleY == angleY)
		return;

	_fSkewAngleX = static_cast<float>(angleX);
	_fSkewAngleY = static_cast<float>(angleY);
	_bTransformNeeded = true;
}

void e2d::Node::setRotation(double angle)
{
	if (_fRotation == angle)
		return;

	_fRotation = static_cast<float>(angle);
	_bTransformNeeded = true;
}

void e2d::Node::setOpacity(double opacity)
{
	if (_fRealOpacity == opacity)
		return;

	_fDisplayOpacity = _fRealOpacity = min(max(static_cast<float>(opacity), 0), 1);
	// 更新节点透明度
	_updateOpacity();
}

void e2d::Node::setPivotX(double pivotX)
{
	this->setPivot(pivotX, _fPivotY);
}

void e2d::Node::setPivotY(double pivotY)
{
	this->setPivot(_fPivotX, pivotY);
}

void e2d::Node::setPivot(double pivotX, double pivotY)
{
	if (_fPivotX == pivotX && _fPivotY == pivotY)
		return;

	_fPivotX = min(max(static_cast<float>(pivotX), 0), 1);
	_fPivotY = min(max(static_cast<float>(pivotY), 0), 1);
	_bTransformNeeded = true;
}

void e2d::Node::setWidth(double width)
{
	this->setSize(width, _fHeight);
}

void e2d::Node::setHeight(double height)
{
	this->setSize(_fWidth, height);
}

void e2d::Node::setSize(double width, double height)
{
	if (_fWidth == width && _fHeight == height)
		return;

	_fWidth = static_cast<float>(width);
	_fHeight = static_cast<float>(height);
	_bTransformNeeded = true;
}

void e2d::Node::setSize(Size size)
{
	this->setSize(size.width, size.height);
}

void e2d::Node::setProperty(NodeProperty prop)
{
	this->setVisiable(prop.visable);
	this->setPos(prop.posX, prop.posY);
	this->setSize(prop.width, prop.height);
	this->setOpacity(prop.opacity);
	this->setPivot(prop.pivotX, prop.pivotY);
	this->setScale(prop.scaleX, prop.scaleY);
	this->setRotation(prop.rotation);
	this->setSkew(prop.skewAngleX, prop.skewAngleY);
}

void e2d::Node::setCollider(ColliderType nColliderType)
{
	switch (nColliderType)
	{
	case ColliderType::RECT:
	{
		auto rect = new ColliderRect(this);
		this->setCollider(rect);
		break;
	}

	case ColliderType::CIRCLE:
	{
		auto rect = new ColliderCircle(this);
		this->setCollider(rect);
		break;
	}

	case ColliderType::ELLIPSE:
	{
		auto rect = new ColliderEllipse(this);
		this->setCollider(rect);
		break;
	}

	default:
		break;
	}
}

void e2d::Node::setCollider(Collider * pCollider)
{
	// 删除旧的碰撞体
	ColliderManager::__removeCollider(_pCollider);
	// 添加新的碰撞体
	ColliderManager::__addCollider(pCollider);

	if (pCollider)
	{
		// 双向绑定
		this->_pCollider = pCollider;
		pCollider->_pParentNode = this;
	}
	else
	{
		this->_pCollider = nullptr;
	}
}

void e2d::Node::addColliableName(const String& collliderName)
{
	unsigned int hash = collliderName.getHashCode();
	_vColliders.insert(hash);
}

#ifdef HIGHER_THAN_VS2012
void e2d::Node::addColliableName(const std::initializer_list<String>& vCollliderName)
{
	for (const auto &name : vCollliderName)
	{
		this->addColliableName(name);
	}
}
#endif

void e2d::Node::removeColliableName(const String& collliderName)
{
	unsigned int hash = collliderName.getHashCode();
	_vColliders.erase(hash);
}

void e2d::Node::addChild(Node * child, int order  /* = 0 */)
{
	WARN_IF(child == nullptr, "Node::addChild NULL pointer exception.");

	if (child)
	{
		ASSERT(child->_pParent == nullptr, "Node already added. It can't be added again!");

		for (Node * parent = this; parent != nullptr; parent = parent->getParent())
		{
			ASSERT(child != parent, "A Node cannot be the child of his own children!");
		}

		_vChildren.push_back(child);

		child->setOrder(order);

		child->retain();

		child->_pParent = this;

		if (this->_pParentScene)
		{
			child->_setParentScene(this->_pParentScene);
		}

		// 更新子节点透明度
		child->_updateOpacity();
		// 更新节点转换
		child->_bTransformNeeded = true;
		// 更新子节点排序
		_bSortChildrenNeeded = true;
	}
}

#ifdef HIGHER_THAN_VS2012
void e2d::Node::addChild(const std::initializer_list<Node*>& vNodes, int order)
{
	for (const auto &node : vNodes)
	{
		this->addChild(node, order);
	}
}
#endif

e2d::Node * e2d::Node::getParent() const
{
	return _pParent;
}

e2d::Scene * e2d::Node::getParentScene() const
{
	return _pParentScene;
}

std::vector<e2d::Node*> e2d::Node::getChildren(const String& name) const
{
	std::vector<Node*> vChildren;
	unsigned int hash = name.getHashCode();

	for (auto child : _vChildren)
	{
		// 不同的名称可能会有相同的 Hash 值，但是先比较 Hash 可以提升搜索速度
		if (child->_nHashName == hash && child->_name == name)
		{
			vChildren.push_back(child);
		}
	}
	return std::move(vChildren);
}

e2d::Node * e2d::Node::getChild(const String& name) const
{
	unsigned int hash = name.getHashCode();

	for (auto child : _vChildren)
	{
		// 不同的名称可能会有相同的 Hash 值，但是先比较 Hash 可以提升搜索速度
		if (child->_nHashName == hash && child->_name == name)
		{
			return child;
		}
	}
	return nullptr;
}

std::vector<e2d::Node*> e2d::Node::getAllChildren() const
{
	return _vChildren;
}

int e2d::Node::getChildrenCount() const
{
	return static_cast<int>(_vChildren.size());
}

void e2d::Node::removeFromParent()
{
	if (_pParent)
	{
		_pParent->removeChild(this);
	}
}

bool e2d::Node::removeChild(Node * child)
{
	WARN_IF(child == nullptr, "Node::removeChildren NULL pointer exception.");

	if (_vChildren.empty())
	{
		return false;
	}

	if (child)
	{
		size_t size = _vChildren.size();
		for (size_t i = 0; i < size; i++)
		{
			if (_vChildren[i] == child)
			{
				_vChildren.erase(_vChildren.begin() + i);
				child->_pParent = nullptr;

				if (child->_pParentScene)
				{
					child->_setParentScene(nullptr);
				}

				child->release();
				return true;
			}
		}
	}
	return false;
}

void e2d::Node::removeChildren(const String& childName)
{
	WARN_IF(childName.isEmpty(), "Invalid Node name.");

	if (_vChildren.empty())
	{
		return;
	}

	// 计算名称 Hash 值
	unsigned int hash = childName.getHashCode();

	size_t size = _vChildren.size();
	for (size_t i = 0; i < size; i++)
	{
		auto child = _vChildren[i];
		if (child->_nHashName == hash && child->_name == childName)
		{
			_vChildren.erase(_vChildren.begin() + i);
			child->_pParent = nullptr;
			if (child->_pParentScene)
			{
				child->_setParentScene(nullptr);
			}
			child->release();
		}
	}
}

void e2d::Node::clearAllChildren()
{
	// 所有节点的引用计数减一
	for (auto child : _vChildren)
	{
		child->release();
	}
	// 清空储存节点的容器
	_vChildren.clear();
}

void e2d::Node::runAction(Action * action)
{
	if (this != action->getTarget())
	{
		ASSERT(action->getTarget() == nullptr, "The action has already got a target!");
		ActionManager::start(action, this, false);
	}
	else
	{
		action->reset();
	}
}

void e2d::Node::resumeAction(const String& strActionName)
{
	auto actions = ActionManager::get(strActionName);
	for (auto action : actions)
	{
		if (action->getTarget() == this)
		{
			action->resume();
		}
	}
}

void e2d::Node::pauseAction(const String& strActionName)
{
	auto actions = ActionManager::get(strActionName);
	for (auto action : actions)
	{
		if (action->getTarget() == this)
		{
			action->pause();
		}
	}
}

void e2d::Node::stopAction(const String& strActionName)
{
	auto actions = ActionManager::get(strActionName);
	for (auto action : actions)
	{
		if (action->getTarget() == this)
		{
			action->stop();
		}
	}
}

e2d::Action * e2d::Node::getAction(const String& strActionName)
{
	auto actions = ActionManager::get(strActionName);
	for (auto action : actions)
	{
		if (action->getTarget() == this)
		{
			return action;
		}
	}
	return nullptr;
}

std::vector<e2d::Action*> e2d::Node::getActions(const String& strActionName)
{
	std::vector<Action*>::iterator iter;
	auto actions = ActionManager::get(strActionName);
	for (iter = actions.begin(); iter != actions.end();)
	{
		if ((*iter)->getTarget() != this)
		{
			iter = actions.erase(iter);
		}
		else
		{
			iter++;
		}
	}
	return std::move(actions);
}

bool e2d::Node::isPointIn(Point point) const
{
	BOOL ret = 0;
	// 如果存在碰撞体，用碰撞体判断
	if (_pCollider)
	{
		_pCollider->getD2dGeometry()->FillContainsPoint(
			D2D1::Point2F(
				static_cast<float>(point.x),
				static_cast<float>(point.y)),
			_MatriFinal,
			&ret
		);
	}
	else
	{
		// 为节点创建一个临时碰撞体
		ID2D1RectangleGeometry * rect;
		Renderer::getID2D1Factory()->CreateRectangleGeometry(
			D2D1::RectF(0, 0, _fWidth, _fHeight),
			&rect
		);
		// 判断点是否在碰撞体内
		rect->FillContainsPoint(
			D2D1::Point2F(
				static_cast<float>(point.x),
				static_cast<float>(point.y)),
			_MatriFinal,
			&ret
		);
		// 删除临时创建的碰撞体
		SafeReleaseInterface(&rect);
	}

	if (ret)
	{
		return true;
	}

	return false;
}

bool e2d::Node::isIntersectWith(const Node * node) const
{
	// 如果存在碰撞体，用碰撞体判断
	if (this->_pCollider && node->_pCollider)
	{
		Relation relation = this->_pCollider->getRelationWith(node->_pCollider);
		if ((relation != Relation::UNKNOWN) && 
			(relation != Relation::DISJOINT))
		{
			return true;
		}
	}
	else
	{
		// 为节点创建一个临时碰撞体
		ID2D1RectangleGeometry * pRect1;
		ID2D1RectangleGeometry * pRect2;
		ID2D1TransformedGeometry * pCollider;
		D2D1_GEOMETRY_RELATION relation;

		// 根据自身大小位置创建矩形
		Renderer::getID2D1Factory()->CreateRectangleGeometry(
			D2D1::RectF(0, 0, _fWidth, _fHeight),
			&pRect1
		);
		// 根据二维矩阵进行转换
		Renderer::getID2D1Factory()->CreateTransformedGeometry(
			pRect1,
			_MatriFinal,
			&pCollider
		);
		// 根据相比较节点的大小位置创建矩形
		Renderer::getID2D1Factory()->CreateRectangleGeometry(
			D2D1::RectF(0, 0, node->_fWidth, node->_fHeight),
			&pRect2
		);
		// 获取相交状态
		pCollider->CompareWithGeometry(
			pRect2,
			node->_MatriFinal,
			&relation
		);
		// 删除临时创建的碰撞体
		SafeReleaseInterface(&pRect1);
		SafeReleaseInterface(&pRect2);
		SafeReleaseInterface(&pCollider);
		if ((relation != D2D1_GEOMETRY_RELATION_UNKNOWN) &&
			(relation != D2D1_GEOMETRY_RELATION_DISJOINT))
		{
			return true;
		}
	}

	return false;
}

void e2d::Node::setAutoUpdate(bool bAutoUpdate)
{
	_bAutoUpdate = bAutoUpdate;
}

void e2d::Node::setDefaultPiovt(double defaultPiovtX, double defaultPiovtY)
{
	s_fDefaultPiovtX = min(max(static_cast<float>(defaultPiovtX), 0), 1);
	s_fDefaultPiovtY = min(max(static_cast<float>(defaultPiovtY), 0), 1);
}

void e2d::Node::setDefaultColliderEnable(bool enable)
{
	s_fDefaultColliderEnabled = enable;
}

void e2d::Node::onDestroy()
{
	ActionManager::__clearAllBindedWith(this);
	ColliderManager::__removeCollider(_pCollider);
	for (auto child : _vChildren)
	{
		SafeRelease(&child);
	}
}

void e2d::Node::resumeAllActions()
{
	ActionManager::__resumeAllBindedWith(this);
}

void e2d::Node::pauseAllActions()
{
	ActionManager::__pauseAllBindedWith(this);
}

void e2d::Node::stopAllActions()
{
	ActionManager::__stopAllBindedWith(this);
}

void e2d::Node::setVisiable(bool value)
{
	_bVisiable = value;
}

void e2d::Node::setName(const String& name)
{
	WARN_IF(name.isEmpty(), "Invalid Node name.");

	if (!name.isEmpty() && _name != name)
	{
		// 保存节点名
		_name = name;
		// 保存节点 Hash 名
		_nHashName = name.getHashCode();
	}
}

void e2d::Node::_setParentScene(Scene * scene)
{
	_pParentScene = scene;
	for (auto child : _vChildren)
	{
		child->_setParentScene(scene);
	}
}