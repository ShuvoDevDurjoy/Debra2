#include "../../../include/GraphEngine/Graphs/GraphObjects/Graphing/GridBox.hpp"

GridBox::GridBox(int row, int column)
{
    this->row = row;
    this->column = column;
    x = -15;
    y = -15;
    width = 30;
    height = 30;
    generateGrid();
}

void GridBox::generateBox()
{
    GraphGroup *border = new GraphGroup();
    border->setColor(this->colors);
    border->add(new Line(glm::vec3(x + width, y, 0), glm::vec3(x, y, 0)));
    border->add(new Line(glm::vec3(x, y, 0), glm::vec3(x, y - height, 0)));
    border->add(new Line(glm::vec3(x, y - height, 0), glm::vec3(x + width, y - height, 0)));
    border->add(new Line(glm::vec3(x + width, y - height, 0), glm::vec3(x + width, y, 0)));
    // border->synchronize(10);
    add(border);
}

void GridBox::setDimension(float x, float y, float width, float height)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    subGraphObjects.clear();
    generateBox();
    generateGrid();
}

void GridBox::generateGrid()
{
    // sub_sync = false;
    // sub_obj_show_interval = 0.0f;
    // generate rows
    GraphGroup *gg = new GraphGroup();
    gg->setColor(this->colors);
    float row_increment = height / row;
    float column_increment = width / column;
    for (int i = 1; i < row; ++i)
    {
        gg->add(new Line(glm::vec3(x, y - (row_increment * i), 0), glm::vec3(x + width, y - (row_increment * i), 0)));
    }

    gg->synchronize(10);

    GraphGroup *columnGroup = new GraphGroup();
    columnGroup->setColor(this->colors);
    for (int i = 1; i < column; ++i)
    {
        columnGroup->add(new Line(glm::vec3(x + (column_increment * i), y, 0), glm::vec3(x + (column_increment * i), y - height, 0)));
    }

    columnGroup->synchronize(10);

        add(gg);
    add(columnGroup);
}