#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <opencv.hpp> 
/*
	基本宏声明
*/
//常用宏Initialization
#define COLOUR_INITIALIZATION_DATA 0
//真彩图特征
#define COLOUR_IMAGE_RGB 3	//多通道
#define COLOUR_IMAGE_RGBA 4	//带透明度的真彩图
//特征库文件名后缀
#define COLOUR_FILE_SUFFIX "colour"	//特征库文件后缀名
//特征库保存方式
#define COLOUR_FILE_SEVE_ADD 0	//以附加的方式保存到文件中,文件必须存在
#define COLOUR_FILE_SEVE_NEW 1	//保存为新文件，如果文件存在则保存失败
#define COLOUR_FILE_SEVE_NEW_FORCE 11	//保存为新文件，如果文件存储则覆盖
//错误宏
int Colour_Errror = 0;				//错误类型，GetERRor函数根据此类型来判断运行的错误状态
char Colour_Error_Func[256] = {0};	//报错宏
#define COLOUR_ERROR_POINTER_NULL -1							//指针为空
#define COLOUR_ERROR_MALLOC_NULL -2								//malloc分配内存失败
#define COLOUR_ERROR_CREATE		-3								//无法创建颜色特征表
#define COLOUR_ERROR_TRANSBOUNDARY	-4							//请求值超出颜色特征表的大小范围
#define COLOUR_ERROR_FILE			-5							//无法打开指定文件
#define COLOUR_ERROR_FILE_READ_0	-6							//读取内容为空
#define COLOUR_ERROR_SUBSCRIPT_0	-7							//下标从0开始
#define COLOUR_ERROR_IMAGE_NULL		-8							//图像指针为空
#define COLOUR_ERROR_IMAGE_RGB		-9							//该图像不是一个真彩图
#define COLOUR_ERROR_FILE_NO_EXISTENT -10						//颜色特征库文件不存在
#define COLOUR_ERROR_FILE_EXISTENT -11							//颜色特征库文件已存在
#define COLOUR_ERROR_FILE_NO_MODE	-12							//没有这个保存方式
#define COLOUR_ERROR_STR_NULL			-13						//传递进来的字符为空
#define COLOUR_ERROR_FILE_INCORRECT		-14						//不是一个正确的颜色特征库文件
#define COLOUR_ERROR_FILE_WRITE	-15								//写入内容为空
#define COLOUR_NORMAL			0								//函数执行正常
//类型宏
#define COLOURIN(TYPE) inline TYPE								//内联函数类型
#define COLOURIN_SIGNED(TYPE) inline signed TYPE				//带符号的内联函数类型
//上限宏
#define COLOUR_MAX_MIN_RGB_ARR_SIZE 3				//MAX_与MIN,RGB数组大小,无需分割开，因为上限与下限必须成正比例
/*
	宏函数
*/
#define	COLOUR_FUNCTION_CONTRAST(a,b)	(a==b)?0:1;	//三目表达式判断是否相等
/*
	颜色特征结构体
*/
typedef struct Colour_Features{//该结构体拥有记录头节点，尾节点,上个元素与下个元素的多样数据结构，支持下标引用
	Colour_Features(int _Val){//构造函数,如果用户需要显示初始化时，则调用该函数！
		memset(Colour_Name, _Val, sizeof(Colour_Name));
		for (int i = 0; i < 3; ++i){
			Colour_Max_RGB[i] = _Val;
			Colour_Min_RGB[i] = _Val;
		}
		Colour_Label = _Val;
	}
	char Colour_Name[256];		//颜色名
	double Colour_Max_RGB[COLOUR_MAX_MIN_RGB_ARR_SIZE];	//最大颜色范围
	double Colour_Min_RGB[COLOUR_MAX_MIN_RGB_ARR_SIZE];	//最小颜色范围
	int Colour_Label = 0;	//用于记录序号,记录当前激活列是哪个
	Colour_Features *Colour_Next = NULL;	//下一个元素

}Colour;
/*
	颜色特征表序列号
*/
int Colour_Serial = 0;
/*
	颜色特征表首节点，这里将其从结构体里分隔开，便于减少结构体内存以及代码开销
*/
Colour_Features *Colour_Top = NULL;	//最后一个压入元素节点
/*
颜色表
*/
typedef struct Colour_Form{
	Colour_Form(int _Val){//构造函数,如果用户需要显示初始化时，则调用该函数！
		memset(Colour_Name, _Val, sizeof(Colour_Name));
	}
	char Colour_Name[256];		//颜色名
	int Colour_Label = 0;		//当前活跃节点
	Colour_Form *Colour_Next = NULL;	//下一个元素

}Form;
/*
颜色表序列号
*/
int Colour_Form_Serial = 0;
/*
用于记录边界元素
*/
Colour_Form *Colour_Form_Top = NULL;
//其他相关操作函数
//获取最近一次执行错误描述信息，并将其显示出来
COLOURIN(void) Get_Colour_Error_State_Print(){
	char Error[256] = { 0 };	//错误信息
	switch (Colour_Errror)
	{
	case COLOUR_ERROR_POINTER_NULL:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "传递进函数的指针为空");
		break;
	case COLOUR_ERROR_MALLOC_NULL:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "malloc函数分配内存失败，计算机中无可用内存");
		break;
	case COLOUR_ERROR_CREATE:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func,"无法创建颜色特征表");
		break;
	case COLOUR_ERROR_TRANSBOUNDARY:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "请求值超出颜色特征表大小范围");
		break;
	case COLOUR_ERROR_FILE:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "无法打开指定文件");
		break;
	case COLOUR_ERROR_FILE_READ_0:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "文件中读取到的内容为空");
		break;
	case COLOUR_ERROR_SUBSCRIPT_0:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "是否在指定下标时从0开始，而不是1？");
		break;
	case COLOUR_ERROR_IMAGE_NULL:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "传递进来的图像指针为空");
		break;
	case COLOUR_ERROR_IMAGE_RGB:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "该图像不是一个真彩图");
		break;
	case COLOUR_ERROR_FILE_NO_EXISTENT:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "图像特征库文件不存在");
		break;
	case COLOUR_ERROR_FILE_EXISTENT:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "图像特征库文件已存在，发生重复错误");
		break;
	case COLOUR_ERROR_FILE_NO_MODE:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "保存文件时，所使用的保存方式不存在");
		break;
	case COLOUR_ERROR_STR_NULL:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "传递进来的指针为空");
		break;
	case COLOUR_ERROR_FILE_INCORRECT:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "不是一个正确的颜色特征库文件");
		break;
	case COLOUR_ERROR_FILE_WRITE:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "写入空内容");
		break;
	default:
		printf("函数执行并没有发生任何错误！");
		break;
	}
	//打印错误信息
	printf(Error);
	
}
//获取最近一次执行错误的描述信息，并以字符串的形式返回
COLOURIN(char*) Get_Colour_Error_State(){
	char Error[256] = { 0 };	//错误信息
	switch (Colour_Errror)
	{
	case COLOUR_ERROR_POINTER_NULL:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "传递进函数的指针为空");
		break;
	case COLOUR_ERROR_MALLOC_NULL:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "malloc函数分配内存失败，计算机中无可用内存");
		break;
	case COLOUR_ERROR_CREATE:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "无法创建颜色特征表");
		break;
	case COLOUR_ERROR_TRANSBOUNDARY:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "请求值超出颜色特征表大小范围");
		break;
	case COLOUR_ERROR_FILE:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "无法打开指定文件");
		break;
	case COLOUR_ERROR_FILE_READ_0:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "文件中读取到的内容为空");
		break;
	case COLOUR_ERROR_SUBSCRIPT_0:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "是否在指定下标时从0开始，而不是1？");
		break;
	case COLOUR_ERROR_IMAGE_NULL:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "传递进来的图像指针为空");
		break;
	case COLOUR_ERROR_IMAGE_RGB:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "该图像不是一个真彩图");
		break;
	case COLOUR_ERROR_FILE_NO_EXISTENT:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "图像特征库文件不存在");
		break;
	case COLOUR_ERROR_FILE_EXISTENT:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "图像特征库文件已存在，发生重复错误");
		break;
	case COLOUR_ERROR_FILE_NO_MODE:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "保存文件时，所使用的保存方式不存在");
		break;
	case COLOUR_ERROR_STR_NULL:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "传递进来的指针为空");
		break;
	case COLOUR_ERROR_FILE_INCORRECT:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "不是一个正确的颜色特征库文件");
		break;
	case COLOUR_ERROR_FILE_WRITE:
		sprintf(Error, "执行错误函数:%s,错误类型：%s!", Colour_Error_Func, "写入空内容");
		break;
	default:
		printf("函数执行并没有发生任何错误！");
		break;
	}
	return Error;
}
/*
颜色特征表相关操作函数
*/
//创建一个新的颜色特征表并初始化
COLOURIN_SIGNED(int) Colour_Features_Create_Initialization(struct Colour_Features** Colour/*要初始化的颜色数据结构*/){	
	//将colour初始化为NULL
	(*Colour) = COLOUR_INITIALIZATION_DATA;
	//为其分配一个内存
	(*Colour) = (struct Colour_Features*)malloc(sizeof(struct Colour_Features));
	if ((*Colour) == NULL){	//判断内存是否分配成功
		strcpy(Colour_Error_Func, "Colour_Features_Create_Initialization");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_MALLOC_NULL;	//当前计算机没有可用内存分批
		return COLOUR_ERROR_MALLOC_NULL;	//结束函数执行
		
	}
	//置空第一个元素的所有节点，让其变成空元素
	//置空Colour_Name
	memset((*Colour)->Colour_Name, COLOUR_INITIALIZATION_DATA, sizeof((*Colour)->Colour_Name));
	//循环置空MAX与MIN
	for (int i = 0; i < 3; ++i){
		(*Colour)->Colour_Max_RGB[i] = COLOUR_INITIALIZATION_DATA;
		(*Colour)->Colour_Min_RGB[i] = COLOUR_INITIALIZATION_DATA;
	}
	//节点指针置空
	(*Colour)->Colour_Label = COLOUR_INITIALIZATION_DATA;
	(*Colour)->Colour_Next = COLOUR_INITIALIZATION_DATA;
	//保存顶部节点
	Colour_Top = (*Colour);
	//函数执行正确
	return COLOUR_NORMAL;
	/*
	备注：当创建一个表后使用其他压入函数压入数值时都会创建一个新的表项，也就是说使用此函数创建的表项首表项为边界表项，next为空用于指明首边界的，而Colour_TOP变量每次在被压入新表项时值都会指向新表项的地址，用于指明尾边界
	思路：为了确保不会发生超出内存范围的中断BUG
	*/

}
//获取结构体大小
COLOURIN_SIGNED(int) Colour_Get_Struct_Size(struct Colour_Features** Colour/*要获取大小的结构体*/){
	if ((*Colour) == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Get_Struct_Size");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	//循环遍历结构体并计数
	int i = 0;
	//保存当前表项位置
	struct Colour_Features *Colour_Current = (*Colour);
	//得到首元素表项
	(*Colour) = Colour_Top;
	//开始从头遍历
	for (; (*Colour)->Colour_Next != NULL; ++i){
		(*Colour) = (*Colour)->Colour_Next;	//如果不是NULL代表还有节点元素
	}
	//节点复原
	(*Colour) = Colour_Current;
	return i;
}
//将一个数据压入颜色特征数据结构体
COLOURIN_SIGNED(int) Colour_Features_Pressing_Basic_Type(struct Colour_Features** Colour/*要压入的颜色数据结构*/, char *Colour_Name/*颜色名*/, double *Colour_Max_RGB/*颜色最大值*/, double *Colour_Min_RGB/*颜色最小值*/){	//以指针形式访问，减少内存开销
	//判断传递进来的指针是否为空
	if ((*Colour) == NULL){//判断指针是否为空，如果为空代表当前结构体指针为空指针
		if (COLOUR_NORMAL == Colour_Features_Create_Initialization(Colour)){//创建一个新的颜色特征表并初始化
			struct Colour_Features *colour;
			if (COLOUR_NORMAL != Colour_Features_Create_Initialization(&colour)){	//判断表单是否创建成功
				strcpy(Colour_Error_Func, "Colour_Features_Pressing_Basic_Type");	//运行出错的函数名
				Colour_Errror = COLOUR_ERROR_MALLOC_NULL;
				return COLOUR_ERROR_MALLOC_NULL;
			}
			//为颜色特征表的表项赋值
			for (int i = 0; i < COLOUR_MAX_MIN_RGB_ARR_SIZE; ++i){	//压入值
				(*Colour)->Colour_Max_RGB[i] = Colour_Max_RGB[i];	//MAX
				(*Colour)->Colour_Min_RGB[i] = Colour_Min_RGB[i];	//MIN
			}
			//压入颜色名
			strcpy(colour->Colour_Name, Colour_Name);
			Colour_Serial = Colour_Serial + 1;
			colour->Colour_Label = Colour_Serial;	//序列号
			//将该节点添加到父表单中
			colour->Colour_Next = (*Colour);
			(*Colour) = colour;
		}
		else{	//无法创建颜色特征表
			strcpy(Colour_Error_Func, "Colour_Features_Pressing_Basic_Type");	//运行出错的函数名
			Colour_Errror = COLOUR_ERROR_CREATE;
			return COLOUR_ERROR_CREATE;
		}

	}
	else{//增加子元素
		//在内存中创建一个颜色特征表
		struct Colour_Features *colour;
		if (COLOUR_NORMAL != Colour_Features_Create_Initialization(&colour)){	//判断表单是否创建成功
			strcpy(Colour_Error_Func, "Colour_Features_Pressing_Basic_Type");	//运行出错的函数名
			Colour_Errror = COLOUR_ERROR_MALLOC_NULL;
			return COLOUR_ERROR_MALLOC_NULL;
		}
		//为颜色特征表的表项赋值
		for (int i = 0; i < COLOUR_MAX_MIN_RGB_ARR_SIZE; ++i){	//压入值
			colour->Colour_Max_RGB[i] = Colour_Max_RGB[i];	//MAX
			colour->Colour_Min_RGB[i] = Colour_Min_RGB[i];	//MIN
		}
		//压入颜色名
		strcpy(colour->Colour_Name, Colour_Name);
		Colour_Serial = Colour_Serial + 1;
		colour->Colour_Label = Colour_Serial;	//序列号
		//将该节点添加到父表单中
		colour->Colour_Next = (*Colour);
		(*Colour) = colour;
	}
	return COLOUR_NORMAL;	//函数执行正常
	
}
//释放颜色特征表
COLOURIN(int) Colour_Delete(struct Colour_Features** Colour/*要释放的颜色特征表*/){
	if ((*Colour) == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Delete");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	//获取颜色特征表的大小
	int Colour_size = Colour_Get_Struct_Size(Colour);
	struct Colour_Features* Colour_New;	//这里申请一个指针用于指向新添加的节点元素，不存储任何值，所以无需分配内存
	//循环删除
	for (int i = 0; i <= Colour_size; ++i){
		//保存下一个节点地址，用于循环迭代
		Colour_New = (*Colour)->Colour_Next;
		//释放内存
		delete(*Colour);
		(*Colour) = NULL;
		//指向下一个节点,指向存储下一个节点的临时指针，这样就形成了循环迭代
		(*Colour) = Colour_New;
	
	}
	Colour_Top = NULL;	//边界复原
	return COLOUR_NORMAL;	//函数执行正常
}
//释放指定颜色特征表
COLOURIN(int) Colour_Delete_Appoint(struct Colour_Features** Colour/*要释放的颜色特征表*/, int Colour_Subscript/*颜色特征表表项下标*/){
	if ((*Colour) == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Delete_Appoint");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	//判断下标是不是从0开始计算的，防止访问边界首节点
	if (Colour_Subscript == 0){
		strcpy(Colour_Error_Func, "Colour_Delete_Appoint");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_SUBSCRIPT_0;
		return COLOUR_ERROR_SUBSCRIPT_0;
	}
	//判断下标与表项子选项是否超出结构体大小范围
	if (Colour_Subscript > Colour_Get_Struct_Size(Colour)){
		strcpy(Colour_Error_Func, "Colour_Delete_Appoint");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_TRANSBOUNDARY;	//越界
		return COLOUR_ERROR_TRANSBOUNDARY;
	}
	int Reduction = 0;	//用于判断表项位置是不是就是要删除的表项位置
	//保存当前表项位置
	struct Colour_Features *Colour_Current = (*Colour);
	//得到尾部表项边界元素位置
	(*Colour) = Colour_Top;
	struct Colour_Features *Colour_Record = NULL;	//用于记录上一个元素
	struct Colour_Features *Colour_Record1 = NULL;	//用于记录下一个元素
	for (int i = Colour_Get_Struct_Size(Colour); i > Colour_Subscript /*无需-1，这里我们让下标从1开始算起 >*/; --i){
		//获取上一个元素指针
		if (i == Colour_Subscript+1){
			Colour_Record = (*Colour);	//保存
		}
		(*Colour) = (*Colour)->Colour_Next;
	}
	//判断当前表项位置是不是要删除的表项位置
	if (Colour_Current->Colour_Label == (*Colour)->Colour_Label){
		Reduction = 1;	//当前表项位置就是要删除的表项位置
	}
	//判断指定表项节点的下一个节点是否为首边界节点
	Colour_Record1 = (*Colour);
	Colour_Record1 = Colour_Record1->Colour_Next;
	if (Colour_Record1->Colour_Next == NULL){	//边界首节点
		//释放指定节点元素
		free((*Colour));
		(*Colour) = NULL;
		//如果是边界首节点即让Colour_Record指向该节点即可
		Colour_Record->Colour_Next = Colour_Record1;
		Colour_Record1 = Colour_Record;
		//让Colour重新指向正确的节点
		if (Reduction == 0){	//节点还原
			(*Colour) = Colour_Current;
		}
		else{	//指向替代原位置后的节点元素
			(*Colour) = Colour_Record;
		}
	}
	else{//不是边界首节点
		//释放指定节点元素
		free((*Colour));
		(*Colour) = NULL;
		//节点元素拼接
		Colour_Record1->Colour_Next = Colour_Record;
		//让Colour重新指向正确的节点
		if (Reduction == 0){	//节点还原
			(*Colour) = Colour_Current;
		}
		else{	//指向替代原位置后的节点元素
			(*Colour) = Colour_Record;
		}
	}
	return COLOUR_NORMAL;

}
//获取颜色特征表中当前活跃表项的序列号
COLOURIN(int) Colour_Get_Struct_Label(struct Colour_Features** Colour/*要获取的颜色特征表的Label*/){
	if ((*Colour) == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Get_Struct_Label");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	return (*Colour)->Colour_Label;	//返回Label
	
}
//迭代器-获取颜色特征表中指定表项元素_Colour_Max_RGB
COLOURIN(double) Colour_Get_Struct_MAX_RGB(struct Colour_Features** Colour/*要获取的颜色特征表的Colour*/, int Colour_Subscript/*颜色特征表表项下标*/, int MAX_Subscript/*表项子选项下标*/){
	if ((*Colour) == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Get_Struct_MAX_RGB");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	//判断下标是不是从0开始计算的，防止访问边界首节点
	if (Colour_Subscript == 0 || MAX_Subscript == 0){
		strcpy(Colour_Error_Func, "Colour_Get_Struct_MAX_RGB");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_SUBSCRIPT_0;
		return COLOUR_ERROR_SUBSCRIPT_0;
	}
	//判断下标与表项子选项是否超出结构体大小范围
	if (Colour_Subscript > Colour_Get_Struct_Size(Colour)){
		strcpy(Colour_Error_Func, "Colour_Get_Struct_MAX_RGB");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_TRANSBOUNDARY;	//越界
		return COLOUR_ERROR_TRANSBOUNDARY;
	}
	//判断范围是否超出数组大小
	if (MAX_Subscript > COLOUR_MAX_MIN_RGB_ARR_SIZE){
		strcpy(Colour_Error_Func, "Colour_Get_Struct_MAX_RGB");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_TRANSBOUNDARY;	//越界
		return COLOUR_ERROR_TRANSBOUNDARY;
	}
	//获取指定颜色值
	//保存当前表项位置
	struct Colour_Features *Colour_Current = (*Colour);
	//得到尾部边界元素位置
	(*Colour) = Colour_Top;
	for (int i = Colour_Get_Struct_Size(Colour); i > Colour_Subscript /*无需-1，这里我们让下标从1开始算起*/; --i){
		(*Colour) = (*Colour)->Colour_Next;
	}
	//节点复原
	//(*Colour) = Colour_Current;	//这里不能节点复原，因为复原的话无法获取到当前值
	//返回获取到的max
	return (*Colour)->Colour_Max_RGB[MAX_Subscript - 1];	//数组下标是从0开始计算的，所以这里我们要-1
}
//迭代器-获取颜色特征表中指定表项元素_Colour_Min_RGB
COLOURIN(double) Colour_Get_Struct_MIN_RGB(struct Colour_Features** Colour/*要获取的颜色特征表的Colour*/, int Colour_Subscript/*颜色特征表表项下标*/, int MIN_Subscript/*表项子选项下标*/){
	if ((*Colour) == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Get_Struct_MIN_RGB");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	//判断下标是不是从0开始计算的，防止访问边界首节点
	if (Colour_Subscript == 0 || MIN_Subscript == 0){
		strcpy(Colour_Error_Func, "Colour_Get_Struct_MIN_RGB");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_SUBSCRIPT_0;
		return COLOUR_ERROR_SUBSCRIPT_0;
	}
	//判断下标与表项子选项是否超出结构体大小范围
	if (Colour_Subscript > Colour_Get_Struct_Size(Colour)){
		strcpy(Colour_Error_Func, "Colour_Get_Struct_MIN_RGB");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_TRANSBOUNDARY;	//越界
		return COLOUR_ERROR_TRANSBOUNDARY;
	}
	//判断范围是否超出数组大小
	if (MIN_Subscript > COLOUR_MAX_MIN_RGB_ARR_SIZE){
		strcpy(Colour_Error_Func, "Colour_Get_Struct_MIN_RGB");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_TRANSBOUNDARY;	//越界
		return COLOUR_ERROR_TRANSBOUNDARY;
	}
	//获取指定颜色值
	//保存当前表项位置
	struct Colour_Features *Colour_Current = (*Colour);
	//得到尾部表项边界元素位置
	(*Colour) = Colour_Top;

	for (int i = Colour_Get_Struct_Size(Colour); i > Colour_Subscript /*无需-1，这里我们让下标从1开始算起*/; --i){
		(*Colour) = (*Colour)->Colour_Next;
	}
	//节点复原
	//(*Colour) = Colour_Current;	//这里不能节点复原，因为复原的话无法获取到当前值
	//返回获取到的min
	return (*Colour)->Colour_Min_RGB[MIN_Subscript - 1];	//数组下标是从0开始计算的，所以这里我们要-1
}
//迭代器-获取颜色特征表中指定表项元素_Colour_Name
COLOURIN(char*) Colour_Get_Struct_Colour_Name(struct Colour_Features** Colour/*要获取的颜色特征表的Colour*/, int Colour_Subscript/*颜色特征表表项下标*/){
	if ((*Colour) == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Get_Struct_Colour_Name");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return (char*)/*消除歧义*/COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	//判断下标是不是从0开始计算的，防止访问边界首节点
	if (Colour_Subscript == 0){
		strcpy(Colour_Error_Func, "Colour_Get_Struct_Colour_Name");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_SUBSCRIPT_0;
		return (char*)/*消除歧义*/COLOUR_ERROR_SUBSCRIPT_0;
	}
	//判断下标与表项子选项是否超出结构体大小范围
	if (Colour_Subscript > Colour_Get_Struct_Size(Colour)){
		strcpy(Colour_Error_Func, "Colour_Get_Struct_Colour_Name");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_TRANSBOUNDARY;	//越界
		return (char*)/*消除歧义*/COLOUR_ERROR_TRANSBOUNDARY;
	}
	//获取指定颜色值
	//保存当前表项位置
	struct Colour_Features *Colour_Current = (*Colour);
	//得到尾部边界表项元素位置
	(*Colour) = Colour_Top;
	for (int i = Colour_Get_Struct_Size(Colour); i > Colour_Subscript/*无需-1，这里我们让下标从1开始算起*/; --i){
		(*Colour) = (*Colour)->Colour_Next;
	}
	//节点复原
	//(*Colour) = Colour_Current;	//这里不能节点复原，因为复原的话无法获取到当前值
	//返回获取到的name
	return (*Colour)->Colour_Name;	//数组下标是从0开始计算的，所以这里我们要-1
}
//迭代器-获取颜色特征表中指定表项元素
COLOURIN(Colour_Features*) Colour_Get_Struct_Data(struct Colour_Features** Colour/*要获取的颜色特征表的Colour*/, int Colour_Subscript/*颜色特征表表项下标*/){
	if ((*Colour) == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Get_Struct_Data");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return (Colour_Features*)/*消除歧义*/COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	//判断下标是不是从0开始计算的，防止访问边界首节点
	if (Colour_Subscript == 0){
		strcpy(Colour_Error_Func, "Colour_Get_Struct_Data");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_SUBSCRIPT_0;
		return (Colour_Features*)/*消除歧义*/COLOUR_ERROR_SUBSCRIPT_0;
	}
	//判断下标与表项子选项是否超出结构体大小范围
	if (Colour_Subscript > Colour_Get_Struct_Size(Colour)){
		strcpy(Colour_Error_Func, "Colour_Get_Struct_Data");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_TRANSBOUNDARY;	//越界
		return (Colour_Features*)/*消除歧义*/COLOUR_ERROR_TRANSBOUNDARY;
	}
	//获取指定颜色值
	//保存当前表项位置
	struct Colour_Features *Colour_Current = (*Colour);
	//得到尾部边界表项
	(*Colour) = Colour_Top;
	//得到指定表项
	for (int i = Colour_Get_Struct_Size(Colour); i > Colour_Subscript/*无需-1，这里我们让下标从1开始算起*/; --i){
		(*Colour) = (*Colour)->Colour_Next;
	}
	//表项COPY
	//将colour初始化为NULL
	struct Colour_Features *colour = COLOUR_INITIALIZATION_DATA;
	//为其分配一个内存
	colour = (struct Colour_Features*)malloc(sizeof(struct Colour_Features));
	strcpy(colour->Colour_Name, (*Colour)->Colour_Name);
	//为颜色特征表的表项赋值
	for (int i = 0; i < COLOUR_MAX_MIN_RGB_ARR_SIZE; ++i){	//压入值
		colour->Colour_Max_RGB[i] = (*Colour)->Colour_Max_RGB[i];	//MAX
		colour->Colour_Min_RGB[i] = (*Colour)->Colour_Min_RGB[i];	//MIN
	}
	//节点复原
	(*Colour) = Colour_Current;
	//返回date
	return colour;
}
//颜色特征库COPY函数_浅拷贝，该Copy函数只会Copy一个子元素的地址
COLOURIN_SIGNED(int) Colour_Copy(struct Colour_Features** Colour/*要Copy的颜色特征表*/,struct Colour_Features** Colour1/*要Copy到的颜色特征表*/){
	if ((*Colour) == NULL || Colour1 == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Copy");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	(*Colour1) = (*Colour);	//Copy
	return COLOUR_NORMAL;	//函数执行正确
}
//获取特征库大小
COLOURIN_SIGNED(int) Colour_Get_State_Size(char *Colour_State_Lib/*要获取的颜色特征库*/){
	if (Colour_State_Lib == NULL){	//判断是否为一个空指针
		strcpy(Colour_Error_Func, "Colour_Get_State_Size");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	int i = 0;	//循环变量
	int size = 0;	//特征库大小
	char a = 0;	//临时值
	for (; Colour_State_Lib[i] != '#'; ++i){
		if (Colour_State_Lib[i] == '}'/*边界符号*/){
			++size;
		}

	}
	return size;
}
//颜色特征库序列化
COLOURIN_SIGNED(int) Colour_Serialize(struct Colour_Features** Colour/*序列化的结构体*/, char *Colour_State_Lib/*要序列化的颜色特征库*/){
	if ((*Colour) == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Serialize");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	if (Colour_State_Lib == NULL){	//判断是否为一个空指针
		strcpy(Colour_Error_Func, "Colour_Serialize");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	//开始遍历循环
	(*Colour) = Colour_Top;	//获取首节点
	char colour_name[256] = { 0 };	//节点颜色名称
	double max_rgb[3] = { 0 };	//节点颜色上限大小
	double min_rgb[3] = { 0 };	//节点颜色下限大小
	char max_str[COLOUR_MAX_MIN_RGB_ARR_SIZE][255] = { 0 };	//字符到整数映射
	char min_str[COLOUR_MAX_MIN_RGB_ARR_SIZE][255] = { 0 };	//字符到整数映射
	int Arr_state = 0;	//遍历状态
	int j = 0;	//循环变量
	int y = 0;	//查找变量计数器
	int hei_max = 0;	//max与min的数组下标
	int hei_min = 0;
	for (int i = 0; i < Colour_Get_State_Size(Colour_State_Lib); ++i){
		for (;; ++j){
			if (Colour_State_Lib[j] == ':'){//找到开始标识符
				switch (Arr_state){
				case 0:{	//获取name
						   y = j+1;	//获取当前循环值，+1跳过分隔符
						   for (int h = 0;h<255/*给定一个值,防止因为找不到结束符造成死循环*/; ++h,y++){
							   if (Colour_State_Lib[y] == '&'){	//结束符
								   Arr_state = Arr_state + 1;
								   break;
							   }
							   else{	
								   colour_name[h] = Colour_State_Lib[y];	//获取颜色名
							   }
						   }
				}
					break;
				case 1:{	//获取max
						   y = j + 1;	//获取当前循环值，+1跳过分隔符
						   int max_inp = 0;	//维度下标
						   for (int k = 0;; ++k,++y){
							   if (Colour_State_Lib[y] == '&'){
								   Arr_state = Arr_state + 1;
								   break;	//结束
							   }
							   if (Colour_State_Lib[y] == ','){	//分隔符
								   hei_max += 1;	//下标递增
								   max_inp = 0;		//第x维下标遍历时必须将下标置为0
								   
							   }
							   max_str[hei_max][max_inp] = Colour_State_Lib[y];	//获取关键数据
							   max_inp = max_inp + 1;
						   }
				}	

					break;
				case 2:{	//获取min
						   y = j + 1;	//获取当前循环值，+1跳过分隔符
						   int max_inp = 0; //维度下标
						   for (int k = 0;; ++k,++y){
							   if (Colour_State_Lib[y] == '&'){
								   Arr_state = Arr_state + 1;
								   break;	//结束
							   }
							   if (Colour_State_Lib[y] == ','){	//分隔符
								   hei_min += 1;	//下标递增
								   max_inp = 0;		//第x维下标遍历时必须将下标置为0

							   }
							   else{
								   min_str[hei_min][max_inp] = Colour_State_Lib[y];	//获取关键数据
								   max_inp = max_inp + 1;
							   }

						   }
						  
				}
					break;
				default:
					break;
				}
			
			}
			if (Colour_State_Lib[j] == '}'){	//结束符
				//将获取到的关键字符数据转化成整数
				for (int i = 0; i < COLOUR_MAX_MIN_RGB_ARR_SIZE; ++i){
					max_rgb[i] = atoi(max_str[i]);	//max
					min_rgb[i] = atoi(min_str[i]);	//min
				}
				Colour_Features_Pressing_Basic_Type(Colour, colour_name, max_rgb, min_rgb);	//压入表项
				Arr_state = 0;	//状态码恢复
				hei_max = 0;	//数组下标置0
				hei_min = 0;	//数组下标置0
				j = j + 1;
				break;
			}
		}
	}
	(*Colour) = Colour_Top;	//返回到首节点
	return COLOUR_NORMAL;
}
/*
颜色表相关操作函数
*/
//创建一个颜色表
COLOURIN_SIGNED(int) Colour_Form_Features_Create_Initialization(struct Colour_Form** _Colour_Form/*要初始化的颜色数据结构*/){
	//将colour初始化为NULL
	(*_Colour_Form) = COLOUR_INITIALIZATION_DATA;
	//为其分配一个内存
	(*_Colour_Form) = (struct Colour_Form*)malloc(sizeof(struct Colour_Form));
	if ((*_Colour_Form) == NULL){	//判断内存是否分配成功
		strcpy(Colour_Error_Func, "Colour_Form_Features_Create_Initialization");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_MALLOC_NULL;	//当前计算机没有可用内存分批
		return COLOUR_ERROR_MALLOC_NULL;	//结束函数执行

	}
	//置空第一个元素的所有节点，让其变成空元素
	//置空Colour_Name
	memset((*_Colour_Form)->Colour_Name, COLOUR_INITIALIZATION_DATA, sizeof((*_Colour_Form)->Colour_Name));
	(*_Colour_Form)->Colour_Next = COLOUR_INITIALIZATION_DATA;
	//保存顶部节点
	Colour_Form_Top = (*_Colour_Form);
	//函数执行正确
	return COLOUR_NORMAL;
	/*
	备注：当创建一个表后使用其他压入函数压入数值时都会创建一个新的表项，也就是说使用此函数创建的表项首表项为边界表项，next为空用于指明首边界的，而Colour_TOP变量每次在被压入新表项时值都会指向新表项的地址，用于指明尾边界
	思路：为了确保不会发生超出内存范围的中断BUG
	*/

}
//获取颜色表大小
COLOURIN_SIGNED(int) Colour_Form_Get_Size(Colour_Form** Colour_Form){
	if ((*Colour_Form) == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Form_Get_Size");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	//循环遍历结构体并计数
	int i = 0;
	//保存当前表项位置
	struct Colour_Form *Colour_Current = (*Colour_Form);
	//得到首元素表项
	(*Colour_Form) = Colour_Form_Top;
	//开始从头遍历
	for (; (*Colour_Form)->Colour_Next != NULL; ++i){
		(*Colour_Form) = (*Colour_Form)->Colour_Next;	//如果不是NULL代表还有节点元素
	}
	//节点复原
	(*Colour_Form) = Colour_Current;
	return i;
}
//获取颜色表，颜色名
COLOURIN(char*) Colour_Form_Get_Name(Colour_Form** Colour_Form, int Colour_Subscript/*颜色表,表项下标*/){
	if ((*Colour_Form) == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Form_Get_Name");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return (char*)/*消除歧义*/COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	//判断下标是不是从0开始计算的，防止访问边界首节点
	if (Colour_Subscript == 0){
		strcpy(Colour_Error_Func, "Colour_Form_Get_Name");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_SUBSCRIPT_0;
		return (char*)/*消除歧义*/COLOUR_ERROR_SUBSCRIPT_0;
	}
	//保存当前表项位置
	struct Colour_Form *Colour_Current = (*Colour_Form);
	//得到首元素表项
	(*Colour_Form) = Colour_Form_Top;
	//开始从头遍历
	for (int i = Colour_Form_Get_Size(Colour_Form); i > Colour_Subscript/*无需-1，这里我们让下标从1开始算起*/; --i){
		(*Colour_Form) = (*Colour_Form)->Colour_Next;	//如果不是NULL代表还有节点元素
	}
	//节点复原
	//(*Colour_Form) = Colour_Current;//这里不能节点复原，因为复原的话无法获取到当前值
	return (*Colour_Form)->Colour_Name;	//返回指定下标name
}
//颜色表释放
COLOURIN(int) Colour_Form_Delete(struct Colour_Form** Colour_Form/*要释放的颜色特征表*/){
	if ((*Colour_Form) == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Form_Delete");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	//获取颜色特征表的大小
	int Colour_size = Colour_Form_Get_Size(Colour_Form);
	struct Colour_Form* Colour_New;	//这里申请一个指针用于指向新添加的节点元素，不存储任何值，所以无需分配内存
	//循环删除
	for (int i = 0; i <= Colour_size; ++i){
		//保存下一个节点地址，用于循环迭代
		Colour_New = (*Colour_Form)->Colour_Next;
		//释放内存
		delete(*Colour_Form);
		(*Colour_Form) = NULL;
		//指向下一个节点,指向存储下一个节点的临时指针，这样就形成了循环迭代
		(*Colour_Form) = Colour_New;

	}
	Colour_Form_Top = NULL;	//边界复原
	return COLOUR_NORMAL;	//函数执行正常
}
//压入数据
COLOURIN_SIGNED(int) Colour_Form_Push(struct Colour_Form** _Colour_Form/*要压入的颜色表*/, char *Colour_Form_Name/*颜色表名*/){
	if ((*_Colour_Form) == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Form_Push");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	if (Colour_Form_Name == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Form_Push");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	//分配一个内存
	Colour_Form* Colour_Form_New = NULL;
	Colour_Form_Features_Create_Initialization(&Colour_Form_New);
	//清空
	memset(Colour_Form_New->Colour_Name, 0, sizeof(Colour_Form_New->Colour_Name));
	//赋值
	strcpy(Colour_Form_New->Colour_Name, Colour_Form_Name);
	//序列号赋值
	Colour_Form_Serial = Colour_Form_Serial + 1;
	Colour_Form_New->Colour_Label = Colour_Form_Serial;
	//首尾拼接
	Colour_Form_New->Colour_Next = (*_Colour_Form);
	(*_Colour_Form) = Colour_Form_New;
	return COLOUR_NORMAL;	//函数执行正常
}
//缩放图像函数
//缩放倍数参数，以0.为单位最大为0.9 从0开始计算
COLOURIN(IplImage*) Colour_Image_Zoom(IplImage* Image/*要缩放的图像*/, double  Zoom_Multiple = 0.1/*要缩放的倍数*/){
	if (Image == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Distinguish_Simple");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return (IplImage*)/*消除歧义*/COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	//缩放图像
	CvSize Image_Size = cvSize(Image->width*Zoom_Multiple, Image->height*Zoom_Multiple);	//设置缩放比例
	IplImage *Image_Zoom = cvCreateImage(Image_Size, Image->depth, Image->nChannels);	//创建一个用于指向缩放后的图像内存
	cvResize(Image, Image_Zoom, CV_INTER_NN);// 缩放图像
	//             CV_INTER_NN - 最近邻插值,  
	//             CV_INTER_LINEAR - 双线性插值 (缺省使用)  
	//             CV_INTER_AREA - 使用象素关系重采样。当图像缩小时候，该方法可以避免波纹出现。  
	/*当图像放大时，类似于 CV_INTER_NN 方法..*/
	//             CV_INTER_CUBIC - 立方插值.  
	return Image_Zoom;	//返回缩放后的图像
}
//颜色特征库识别函数_简单识别_自然数RGB比对_常规对比法,优点：检索速度中等，缺点：检索比较笨拙
COLOURIN(Colour_Form*) Colour_Distinguish_Simple(struct Colour_Features** Colour/*识别的颜色特征库*/, IplImage* Image/*要识别颜色的图像*/){
	if ((*Colour) == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Distinguish_Simple");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return (Colour_Form*)/*消除歧义*/COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	if (Image == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Distinguish_Simple");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_IMAGE_NULL;	//传递进来的指针为空
		return (Colour_Form*)/*消除歧义*/COLOUR_ERROR_IMAGE_NULL;	//结束函数执行
	}
	//判断是否是一个多通道RGB真彩图
	if (Image->nChannels != COLOUR_IMAGE_RGB){
		strcpy(Colour_Error_Func, "Colour_Distinguish_Simple");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_IMAGE_RGB;	//不是一个真彩图
		return (Colour_Form*)/*消除歧义*/COLOUR_ERROR_IMAGE_RGB;	//结束函数执行
	}
	//开始识别
	//创建颜色表
	Colour_Form* Colour_Form_New = NULL;
	int Repeat = 0;	//用于判断颜色表是否重复
	//初始化
	Colour_Form_Features_Create_Initialization(&Colour_Form_New);
	int Colour_Size = Colour_Get_Struct_Size(Colour);	//获取颜色特征库数据结构体大小
	CvScalar scalar;    //scalar 
	for (int i = 0; i <= Image->height - 1; ++i){	//-1是因为堆栈sp指针指向地址下标是从0开始的
		for (int j = 0; j <= Image->width - 1; ++j){
			scalar = cvGet2D(Image, i, j);    //获取像素点的RGB颜色分量  
			(*Colour) = Colour_Top;	//恢复到顶部边界节点
			for (int k = 0; k <= Colour_Size; ++k){
				//第一种自然数比对算法
				if ((scalar.val[2] >= (*Colour)->Colour_Min_RGB[0] && scalar.val[1] >= (*Colour)->Colour_Min_RGB[1] && scalar.val[0] >= (*Colour)->Colour_Min_RGB[2]) ||
					(scalar.val[2] > (*Colour)->Colour_Min_RGB[0] && scalar.val[1] > (*Colour)->Colour_Min_RGB[1] && scalar.val[0] > (*Colour)->Colour_Min_RGB[2])
					&& (scalar.val[2] <= (*Colour)->Colour_Max_RGB[0] && scalar.val[1] <= (*Colour)->Colour_Max_RGB[1] && scalar.val[0] <= (*Colour)->Colour_Max_RGB[2]) ||
					(scalar.val[2] < (*Colour)->Colour_Max_RGB[0] && scalar.val[1] < (*Colour)->Colour_Max_RGB[1] && scalar.val[0] < (*Colour)->Colour_Max_RGB[2])){
					
					//压入之前判断一下是否重复
					if (Colour_Form_New != NULL){	//判断颜色表里是否有已经获取到的颜色
						Colour_Form_New = Colour_Form_Top;
						for (int i = 0; i < Colour_Form_Get_Size(&Colour_Form_New); ++i){
							//判断是否一致
							if (strcmp((*Colour)->Colour_Name, Colour_Form_New->Colour_Name) == 0){
								Repeat = 1;	//设置重复标志
								Colour_Form_New = Colour_Form_Top;	//边界节点恢复
								break;
							}
							//校验下一个
							Colour_Form_New = Colour_Form_New->Colour_Next;
						}
						if (Repeat == 0){
							//恢复到尾部边界
							Colour_Form_New = Colour_Form_Top;
							//压入颜色特征
							if (COLOUR_NORMAL != Colour_Form_Push(&Colour_Form_New, (*Colour)->Colour_Name)){	//将对应的颜色压入
								return (Colour_Form*)-1;	//压入失败
							}
							else{
								break;	//获取下一个像素值
							}
						}
						else{
							Repeat = 0;	//重复标志位置空
							break;
						}
					}

				}
				(*Colour) = (*Colour)->Colour_Next;
			}
		}

	}
	(*Colour) = Colour_Top;
	return Colour_Form_New;	//将获取到的颜色表返回
}
//颜色特征库识别函数_简单识别_自然数RGB比对_缩小图像对比法 优点：检索快，缺点：缩放倍数越小时可能会造成部分颜色丢失，且当根据缩放倍数缩放速率会有所提升，所以建议使用默认值
//缩放倍数参数，以0.为单位最大为0.9 从0开始计算，缩放的倍数越小检索速度越快
COLOURIN(Colour_Form*) Colour_Distinguish_Simple_Zoom(struct Colour_Features** Colour/*识别的颜色特征库*/, IplImage* Image/*要识别颜色的图像*/, double  Zoom_Multiple = 0.1/*要缩放的倍数，建议使用默认值*/){
	if ((*Colour) == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Distinguish_Simple");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return (Colour_Form*)/*消除歧义*/COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	if (Image == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Distinguish_Simple");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_IMAGE_NULL;	//传递进来的指针为空
		return (Colour_Form*)/*消除歧义*/COLOUR_ERROR_IMAGE_NULL;	//结束函数执行
	}
	//判断是否是一个多通道RGB真彩图
	if (Image->nChannels != COLOUR_IMAGE_RGB){
		strcpy(Colour_Error_Func, "Colour_Distinguish_Simple");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_IMAGE_RGB;	//不是一个真彩图
		return (Colour_Form*)/*消除歧义*/COLOUR_ERROR_IMAGE_RGB;	//结束函数执行
	}
	IplImage *Image_Zoom = Colour_Image_Zoom(Image, Zoom_Multiple);
	//开始识别
	//创建颜色表
	Colour_Form* Colour_Form_New = NULL;
	int Repeat = 0;	//用于判断颜色表是否重复
	//初始化
	Colour_Form_Features_Create_Initialization(&Colour_Form_New);
	int Colour_Size = Colour_Get_Struct_Size(Colour);	//获取颜色特征库数据结构体大小
	CvScalar scalar;    //scalar 
	for (int i = 0; i <= Image_Zoom->height - 1; ++i){	//-1是因为堆栈sp指针指向地址下标是从0开始的
		for (int j = 0; j <= Image_Zoom->width - 1; ++j){
			scalar = cvGet2D(Image_Zoom, i, j);    //获取像素点的RGB颜色分量  
			(*Colour) = Colour_Top;	//恢复到顶部边界节点
			for (int k = 0; k <= Colour_Size; ++k){
				//第一种自然数比对算法
				if ((scalar.val[2] >= (*Colour)->Colour_Min_RGB[0] && scalar.val[1] >= (*Colour)->Colour_Min_RGB[1] && scalar.val[0] >= (*Colour)->Colour_Min_RGB[2]) ||
					(scalar.val[2] > (*Colour)->Colour_Min_RGB[0] && scalar.val[1] > (*Colour)->Colour_Min_RGB[1] && scalar.val[0] > (*Colour)->Colour_Min_RGB[2])
					&& (scalar.val[2] <= (*Colour)->Colour_Max_RGB[0] && scalar.val[1] <= (*Colour)->Colour_Max_RGB[1] && scalar.val[0] <= (*Colour)->Colour_Max_RGB[2]) ||
					(scalar.val[2] < (*Colour)->Colour_Max_RGB[0] && scalar.val[1] < (*Colour)->Colour_Max_RGB[1] && scalar.val[0] < (*Colour)->Colour_Max_RGB[2])){

					//压入之前判断一下是否重复
					if (Colour_Form_New != NULL){	//判断颜色表里是否有已经获取到的颜色
						Colour_Form_New = Colour_Form_Top;
						for (int i = 0; i < Colour_Form_Get_Size(&Colour_Form_New); ++i){
							//判断是否一致
							if (strcmp((*Colour)->Colour_Name, Colour_Form_New->Colour_Name) == 0){
								Repeat = 1;	//设置重复标志
								Colour_Form_New = Colour_Form_Top;	//边界节点恢复
								break;
							}
							//校验下一个
							Colour_Form_New = Colour_Form_New->Colour_Next;
						}
						if (Repeat == 0){
							//恢复到尾部边界
							Colour_Form_New = Colour_Form_Top;
							//压入颜色特征
							if (COLOUR_NORMAL != Colour_Form_Push(&Colour_Form_New, (*Colour)->Colour_Name)){	//将对应的颜色压入
								return (Colour_Form*)-1;	//压入失败
							}
							else{
								break;	//获取下一个像素值
							}
						}
						else{
							Repeat = 0;	//重复标志位置空
							break;
						}
					}

				}
				(*Colour) = (*Colour)->Colour_Next;
			}
		}

	}
	(*Colour) = Colour_Top;
	return Colour_Form_New;	//将获取到的颜色表返回
}
/*
颜色特征库相关操作函数
*/
//校验这是否是一个真正的颜色特征库
COLOURIN_SIGNED(int) Colour_State_File_Check(char *Colour_State_Name/*颜色文件名*/){
	//判断是否为一个空指针
	if (Colour_State_Name == NULL){	//判断是否为一个空指针
		strcpy(Colour_Error_Func, "Colour_State_File_Check");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	//判断路径是否有效
	if (0 == strlen(Colour_State_Name)){
		strcpy(Colour_Error_Func, "Colour_State_File_Check");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_STR_NULL;	//空字符
		return COLOUR_ERROR_STR_NULL;	//结束函数执行
	}
	//校验文件名是否正确
	char Colour_Name_Str[] = COLOUR_FILE_SUFFIX;	//获取后缀
	int Colour_Name_Size = strlen(Colour_Name_Str);	//获取后缀名大小
	int Path_Size = strlen(Colour_State_Name);	//完整路径大小
	//比对
	for (int i = Colour_Name_Size; i > 0; --i, --Path_Size){
		if (Colour_Name_Str[i] != Colour_State_Name[Path_Size]){
			strcpy(Colour_Error_Func, "Colour_State_File_Check");	//运行出错的函数名
			Colour_Errror = COLOUR_ERROR_FILE_INCORRECT;	//不是一个正确颜色特征库文件
			return COLOUR_ERROR_FILE_INCORRECT;		//结束函数执行
		}
	}	
	return COLOUR_NORMAL;	//函数执行正常

}
//将现有的颜色特征库保存到本地
COLOURIN_SIGNED(int) Colour_State_File_Seve(char *Colour_State_Lib/*颜色特征库*/, char *Path/*要保存的路径*/, int Mode = COLOUR_FILE_SEVE_NEW_FORCE/*要保存的方式*/){
	//判断传递进来的指针是否为空
	if (Colour_State_Lib == NULL){	//判断是否为一个空指针
		strcpy(Colour_Error_Func, "Colour_State_File_Seve");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	if (Path == NULL){	//判断是否为一个空指针
		strcpy(Colour_Error_Func, "Colour_State_File_Seve");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	if (COLOUR_ERROR_FILE_INCORRECT == Colour_State_File_Check(Path)){	//判断文件名是否正确
		strcpy(Colour_Error_Func, "Colour_State_File_Seve");	//运行出错的函数名
		return -1;	//不是一个正确的颜色特征库文件
	}
	//申请文件指针
	FILE *fp = NULL;
	//校验保存方式
	switch (Mode){
	case COLOUR_FILE_SEVE_ADD:	//以附加的方式保存文件
		fp = fopen(Path, "r");	//先以只读的方式打开，确定文件存在！
		if (fp == NULL){
			strcpy(Colour_Error_Func, "Colour_State_File_Seve");	//运行出错的函数名
			Colour_Errror = COLOUR_ERROR_FILE_NO_EXISTENT;	//文件不存在
			return COLOUR_ERROR_FILE_NO_EXISTENT;	//结束函数执行
		}
		else{//文件存在则重新打开一次
			fclose(fp);
			fp = NULL;
			fp = fopen(Path, "a");
		}
		break;
	case COLOUR_FILE_SEVE_NEW:		//以新文件的方式保存
		fp = fopen(Path, "r");	//先以只读的方式打开，确定文件是否存在！
		if (fp != NULL){
			strcpy(Colour_Error_Func, "Colour_State_File_Seve");	//运行出错的函数名
			Colour_Errror = COLOUR_ERROR_FILE_EXISTENT;	//颜色特征文件已存在
			return COLOUR_ERROR_FILE_EXISTENT;	//结束函数执行
		}
		else{//不存在的话则创建新文件并保存
			fclose(fp);
			fp = NULL;
			fp = fopen(Path, "w");
		}
		break;
	case COLOUR_FILE_SEVE_NEW_FORCE:		//简历新文件
		fp = fopen(Path, "w");
		break;
	default:
		strcpy(Colour_Error_Func, "Colour_State_File_Seve");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_FILE_NO_MODE;	//错误的保存方式
		return COLOUR_ERROR_FILE_NO_MODE;	//结束函数执行
		break;
	}
	//写入
	fwrite(Colour_State_Lib, strlen(Colour_State_Lib), 1, fp);
	//释放
	fclose(fp);
	fp = NULL;
	return COLOUR_NORMAL;	//函数执行正常
}
//将现有的颜色特征库保存到本地_序列化的方式
COLOURIN_SIGNED(int) Colour_Struct_File_Seve(struct Colour_Features** Colour/*要保存的颜色特征库*/, char *Path/*要保存的路径*/){
	//判断传递进来的指针是否为空
	if ((*Colour) == NULL){	//判断是否为一个空指针
		strcpy(Colour_Error_Func, "Colour_Struct_File_Seve");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	if (Path == NULL){	//判断是否为一个空指针
		strcpy(Colour_Error_Func, "Colour_Struct_File_Seve");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	if (COLOUR_ERROR_FILE_INCORRECT == Colour_State_File_Check(Path)){	//判断文件名是否正确
		strcpy(Colour_Error_Func, "Colour_Struct_File_Seve");	//运行出错的函数名
		return -1;	//不是一个正确的颜色特征库文件
	}
	//申请文件指针
	FILE *fp = NULL;
	//保存方式
	fp = fopen(Path, "a");
	char File_[256] = { 0 };	//文件内容
	//写入Colour{COLOUR:黑色&MAX:0,0,0&MIN:0,0,0&}{COLOUR:白色&MAX:255,255,255&MIN:255,255,255&}#
	struct Colour_Features *Colour_Current = (*Colour);	//保存当前节点
	(*Colour) = Colour_Top;	//得到边界元素
	for (int i = 1; i <= Colour_Get_Struct_Size(Colour); ++i){
		sprintf(File_, "{COLOUR:%s&MAX:%d,%d,%d&MIN:%d,%d,%d&}", Colour_Get_Struct_Colour_Name(Colour, i), Colour_Get_Struct_MAX_RGB(Colour, i, 1), \
			Colour_Get_Struct_MAX_RGB(Colour, i, 2), Colour_Get_Struct_MAX_RGB(Colour, i, 3), Colour_Get_Struct_MIN_RGB(Colour, i, 1), Colour_Get_Struct_MIN_RGB(Colour, i, 2), \
			Colour_Get_Struct_MIN_RGB(Colour, i, 3));//格式化字符
		if (0 == fwrite(File_, strlen(File_), 1, fp)){//以附加的方式写入
			strcpy(Colour_Error_Func, "Colour_Struct_File_Seve");	//运行出错的函数名
			Colour_Errror = COLOUR_ERROR_FILE_WRITE;	//写入内容为空
			return COLOUR_ERROR_FILE_WRITE;	//结束函数执行
		}	
		(*Colour) = (*Colour)->Colour_Next;	//下一个
	}
	fwrite("#", strlen("#"), 1, fp);	//写入结束符
	//节点复原
	(*Colour) = Colour_Current;
	//释放
	fclose(fp);
	fp = NULL;
	return COLOUR_NORMAL;	//函数执行正常
}
//颜色特征库实例化_从本地文件加载
COLOURIN_SIGNED(int) Colour_Serialize_Path(struct Colour_Features** Colour/*序列化的结构体*/, char *Colour_State_Lib_Path/*要序列化的颜色特征库路径*/){
	if ((*Colour) == NULL){	//判断是否为一个空结构体
		strcpy(Colour_Error_Func, "Colour_Serialize_Path");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	if (Colour_State_Lib_Path == NULL){	//判断是否为一个空指针
		strcpy(Colour_Error_Func, "Colour_Serialize_Path");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	if (COLOUR_ERROR_FILE_INCORRECT == Colour_State_File_Check(Colour_State_Lib_Path)){	//判断文件名是否正确
		strcpy(Colour_Error_Func, "Colour_Serialize_Path");	//运行出错的函数名
		return -1;	//不是一个正确的颜色特征库文件
	}
	FILE *fp = NULL;	//文件指针
	fp = fopen(Colour_State_Lib_Path, "r");
	if (fp == NULL){
		Colour_Errror = COLOUR_ERROR_FILE;
		return COLOUR_ERROR_FILE;
	}
	//获取文件长度用于创建存储空间
	fseek(fp, 0, SEEK_END); //定位到文件末 
	int nFileLen = ftell(fp); //文件长度
	char *file_str = (char*)malloc(nFileLen);	//创建存储空间
	if (0 == fread(file_str, nFileLen, 1, fp)){	//将文件内容读取到存储空间中
		strcpy(Colour_Error_Func, "Colour_Serialize_Path");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_FILE_READ_0;
		return COLOUR_ERROR_FILE_READ_0;
	}
	fclose(fp);	//关闭文件指针
	//调用颜色特征库实例化函数实例化
	Colour_Serialize(Colour, file_str);
	return COLOUR_NORMAL;	//函数正常执行

}
//颜色特征库编辑器
COLOURIN_SIGNED(int) Colour_State_File_Edit(char *Path/*颜色特征库路径*/){
	if (Path == NULL){	//判断是否为一个空指针
		strcpy(Colour_Error_Func, "Colour_State_File_Edit");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_POINTER_NULL;	//传递进来的指针为空
		return COLOUR_ERROR_POINTER_NULL;	//结束函数执行
	}
	//判断路径是否有效
	if (0 == strlen(Path)){
		strcpy(Colour_Error_Func, "Colour_State_File_Edit");	//运行出错的函数名
		Colour_Errror = COLOUR_ERROR_STR_NULL;	//空字符
		return COLOUR_ERROR_STR_NULL;	//结束函数执行
	}
	if (COLOUR_ERROR_FILE_INCORRECT == Colour_State_File_Check(Path)){	//判断文件名是否正确
		strcpy(Colour_Error_Func, "Colour_State_File_Edit");	//运行出错的函数名
		return -1;	//不是一个正确的颜色特征库文件
	}
	//序列化
	Colour_Features* Colour;	//颜色特征表
	Colour_Features_Create_Initialization(&Colour);	//初始化
	if (COLOUR_NORMAL != Colour_Serialize_Path(&Colour, Path)){
		strcpy(Colour_Error_Func, "Colour_State_File_Edit");	//运行出错的函数名
		return -1;	//无法序列化颜色特征库
	}
	//载入编辑器
	iuns:
	printf("****************************************\n");
	printf("颜色特征库编辑器v2.0\n");
	printf("****************************************\n");
	printf("*颜色特征库信息：\n");
	printf("颜色特征库大小：%d\n", Colour_Get_Struct_Size(&Colour));
	printf("请选择要进行的操作:\n");
	printf("1.删除指定颜色表\n");
	printf("2.保存特征库\n");
	ins:
	printf("键代码：%d");
	int J_Dm = 0;	//选项
	int Sequence = 0;	//序列
	char Colour_File[256] = { 0 };	//保存文件路径
	scanf("%d", &J_Dm);
	switch (J_Dm){
	case 1:
		printf("请输入要删除的序列：");
		scanf("%d", &Sequence);
		if (COLOUR_NORMAL != Colour_Delete_Appoint(&Colour, Sequence)){
			strcpy(Colour_Error_Func, "Colour_State_File_Edit");	//运行出错的函数名
			return -1;	//无法删除特征库指定颜色特征
		}
		printf("删除成功!\n");
		goto iuns;	//回到首界面
		break;
	case 2:
		printf("请输入要保存的文件路径:");
		scanf("%s", Colour_File);
		if (COLOUR_NORMAL != Colour_Struct_File_Seve(&Colour, Colour_File)){	//保存
			strcpy(Colour_Error_Func, "Colour_State_File_Edit");	//运行出错的函数名
			return -1;	//无法将颜色特征库保存到文件
		}

		printf("保存成功!\n");
		goto iuns;
		break;
	default:
		printf("不是正确的键代码!");
		goto ins;
		break;
	}
}


//测试代码
int main(){
	//申请一个颜色特征结构体
	Colour *sd = NULL;
	//设置一个颜色特征库
	char colour_state[] = { "{COLOUR:黑色&MAX:0,0,0&MIN:0,0,0&}{COLOUR:白色&MAX:255,255,255&MIN:255,255,255&}#" };
	//初始化颜色特征结构体
	Colour_Features_Create_Initialization(&sd);
	//序列化颜色特征结构体
	Colour_Serialize(&sd, colour_state);
	//申请一个颜色表
	Colour_Form *colo = NULL;
	//初始化颜色表
	Colour_Form_Features_Create_Initialization(&colo);
	//将要识别的图像文件加载到内存
	IplImage *img = cvLoadImage("d:\\test.jpg");
	if (img == NULL){
		printf("无法打开图像文件");
		getchar();
		return -1;
	}
	//识别颜色
	colo = Colour_Distinguish_Simple_Zoom(&sd, img);
	//打印识别到的颜色
	printf("识别完成，获取到：%d个颜色\n", Colour_Form_Get_Size(&colo));
	for (int i = 0; i < Colour_Form_Get_Size(&colo); ++i){
		printf("第%d个颜色：%s\n", i+1,Colour_Form_Get_Name(&colo, i + 1/*下标从1开始*/));
	}
	//获取最近一次执行错误的函数以及错误类型,并打印
	Get_Colour_Error_State_Print();
	getchar();

}