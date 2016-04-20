#include "Grid.h"

#define SET_DUMMY(node) {	auto dummyTile = make_unique<Tile>(0, *dummySprite); \
							dummyTile->InitializeSprite();\
							node->SetTile(move(dummyTile)); }

using namespace std;

static unique_ptr<Sprite> dummySprite = nullptr;

Grid::Grid(unique_ptr<Sprite> mouseClickComparator_) :
	mouseClickComparator{ move(mouseClickComparator_) }{
	if (dummySprite == nullptr)
		dummySprite = make_unique<Sprite>(Rect{ 0, 0, 64, 32 });
}

Grid::~Grid(){
	vector<Node*> nodes;
	GridTraversal g{ *this };

	while (g.HasNext()){
		nodes.push_back(g.Next());
	}

	while (!nodes.empty()){
		delete nodes.back();
		nodes.pop_back();
	}
}

void Grid::Create(int sizeX, int sizeY){

	root = CreateBlock(sizeX, sizeY);
	center = root;
	view = root;
}

void Grid::Render(const Rect& field){

	Node* curr = view;
	int drawX = field.x;
	int drawY = field.y;
	bool fullFirst = false;

	if (advanceAll)
		drawX += 64;

	//special case row over view node
	if (curr->GetNortheast() != nullptr)
		curr->GetNortheast()->RenderRow(drawX + 32, drawY - 16, field.w);

	while (drawY < field.h){

		//special case node left to first, with avoidance of overwriting first node of  preceding row
		if (!fullFirst && curr->GetNorthwest() != nullptr)
			curr->GetNorthwest()->Render(drawX - 32, drawY - 16);

		curr->RenderRow(drawX, drawY, field.w);

		if (curr == view || drawY + 16 >= field.h){
			Renderer::Instance().SetColor(RGBAColor{ 0xFF, 0, 0, 0xFF });
			Renderer::Instance().DrawFillRect(Rect{ drawX + 32 - 5, drawY + 16 - 5, 10, 10 });
		}

		if (fullFirst){
			curr = curr->GetSouthwest();
			drawX -= 32;
		}
		else{
			curr = curr->GetSoutheast();
			drawX += 32;
		}

		if (curr == nullptr)
			break;

		fullFirst = !fullFirst;
		drawY += 16;
	}

	//special case Node in bottom left corner (take NW of the first row out of bounds, equals to W of first node in last drawn row, but without repetitive checks in loop
	if (curr != nullptr && curr->GetNorthwest() != nullptr)
		curr->GetNorthwest()->Render(drawX - 32, drawY - 16);

}

Node* Grid::CreateBlock(int sizeX, int sizeY){
	Node* myRoot = nullptr;
	Node* curr = nullptr;
	auto rowTop = make_unique<vector<Node*>>();
	auto rowBot = make_unique<vector<Node*>>();

	cout << "create block" << endl;
	for (int x = 0; x < sizeX; x++){
		rowTop->push_back(new Node(x, 0));
	}
	curr = rowTop->front();
	myRoot = curr;

	for (int y = 1; y < sizeY; y++){
		for (int x = 0; x < sizeX; x++){
			rowBot->push_back(new Node(x, y));
		}
		curr = LinkRows(*rowTop, *rowBot);
		rowTop = move(rowBot);
		rowBot = make_unique<vector<Node*>>();
	}
	return myRoot;
}

Node* Grid::LinkRows(vector<Node*>& top, vector<Node*>& bot){
	decltype(top.size()) n = 0;
	bool advanced = (top[0]->GetY() % 2) != 0;

	while (n < top.size()){
		if (advanced){
			top[n]->LinkWithSouthwest(bot[n]);
			if (n + 1 < bot.size())
				top[n]->LinkWithSoutheast(bot[n + 1]);

		}
		else{
			top[n]->LinkWithSoutheast(bot[n]);
			if (n >= 1)
				top[n]->LinkWithSouthwest(bot[n - 1]);
		}
		n++;
	}
	return bot.front();
}

void Grid::CenterToScreen(int screenX, int screenY, const Rect& boundaries){

	RGBAColor col = mouseClickComparator->PixelAt(screenX % 64, screenY % 32);

	int relX = screenX / 64;
	int relY = 2 * (screenY / 32);

	switch (col.r){
		case 107:
			relX--;
			relY--;
			break;
		case 159:
			relY--;
			break;
		case 71:
			relY++;
			break;
		case 167:
			relX--;
			relY++;
		case 27:
			break;
		default:
			cout << "DEFAULT SHOULD NOT HAPPEN LEL: " << (int)col.r << ", " << (int)col.g << ", " << (int)col.b << ", " << (int)col.a << endl;
			break;
	}

	//cout << "SQUARE: " << (screenX / 64) << ", " << (screenY / 32) << endl;

	int diffX = relX - center->GetX() + view->GetX();
	int diffY = relY - center->GetY() + view->GetY();
	//cout << "x = " << relX << ", y = " << relY << ", diff: " << diffX << ", " << diffY << endl;
	center = GoRelative(center, diffX, diffY);


	SET_DUMMY(center);
	AlignView(center, boundaries);

	cout << "center @ " << center->GetX() << ", " << center->GetY() << endl;
	cout << "view @ " << view->GetX() << ", " << view->GetY() << endl;
}

Node* Grid::GoRelative(Node* node, int x, int y){

	static Sprite s{ Sprite(Rect{ 0, 0, 64, 32 }) };

	while (x > 0){
		if (node->GetEast() != nullptr)
			node = node->GetEast();
		else
			break;
		x--;
	}

	while (x < 0){
		if (node->GetWest() != nullptr)
			node = node->GetWest();
		else
			break;
		x++;
	}

	bool advanced = node->GetY() % 2 != 0;
	while (y > 0){
		if (advanced){
			if (node->GetSouthwest() != nullptr)
				node = node->GetSouthwest();
			else
				break;
		}
		else{
			if (node->GetSoutheast() != nullptr)
				node = node->GetSoutheast();
			else
				break;
		}
		advanced = !advanced;
		y--;
	}

	while (y < 0){
		if (advanced){
			if (node->GetNorthwest() != nullptr)
				node = node->GetNorthwest();
			else
				break;
		}
		else{
			if (node->GetNortheast() != nullptr)
				node = node->GetNortheast();
			else
				break;
		}
		advanced = !advanced;
		y++;
	}
	return node;
}

void Grid::AlignView(Node* node, const Rect& field){
	int x = field.w / 2;
	int y = field.h / 2;

	while (x >= 64){
		if (node->GetWest() != nullptr)
			node = node->GetWest();
		else
			break;
		x -= 64;
	}


	while (y >= 32){
		if (node->GetNorth() != nullptr)
			node = node->GetNorth();
		else
			break;
		y -= 32;
	}

	//if view is on top boundary try to get the NW node as view
	if (node->GetNorth() == nullptr){
		if (node->GetNorthwest() != nullptr)
			node = node->GetNorthwest();
	}
	else if (node->GetWest() != nullptr){		//if view is not on any boundary, get SE node as view
		node = node->GetSoutheast();
	}

	if (node->IsOdd() && node->GetWest() == nullptr){
		advanceAll = true;
	}
	else
		advanceAll = false;

	cout << "left: " << x << ", " << y << endl;
	view = node;
}

void Grid::AlignViewToCenter(const Rect& field){
	AlignView(center, field);
}

GridTraversal::GridTraversal(Grid& grid) :
	curr{ grid.root },
	rowFirst{ grid.root }{

	if (curr->GetY() % 2 != 0)
		advanced = true;
	else
		advanced = false;
}

GridTraversal::~GridTraversal(){

}

Node* GridTraversal::Next(void){
	auto currReturn = curr;

	curr = curr->GetEast();
	if (curr == nullptr){
		if (advanced)
			curr = rowFirst->GetSouthwest();
		else
			curr = rowFirst->GetSoutheast();
		rowFirst = curr;
		advanced = !advanced;
	}
	return currReturn;
}