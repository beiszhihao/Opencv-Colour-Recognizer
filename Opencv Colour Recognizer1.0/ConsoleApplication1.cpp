/*颜色识别器
*版本：v1.0
*作者：周志豪
*3.28-4.16 15：59
*/
//该库需要Opencv2.4.9的支持！
#include <stdio.h>
#include <opencv.hpp> 
#include <stdlib.h>

//file
#define FILE_NAME_MIX 256

//colour file
#define COLOURFILE ".colour"

//colour typedef define
#define COLOURIN(TYPE) inline TYPE

//free
#define PASS	NULL;

//colour typedef bool
typedef unsigned char	COLOUR_BOOL;	//-125~125
#define COLOUR_BOOL_FALSE 1
#define COLOUR_BOOL_TRUE 0
#define COLOUR_BOOL_NOT 3

//machine_learning_colour_train
#define KEY_FILE "key" COLOURFILE
//colour error
typedef struct colour_error{
	char error_type[256];
	COLOUR_BOOL colour_sign = COLOUR_BOOL_FALSE;

}colour_error;

colour_error colour_error_type;
#define COLOUR_ERROR_NULL "传递进来的指针没有为其分配内存"
#define COLOUR_ERROR_FILE_NULL "无法打开指定文件"
#define COLOUR_ERROR_FILE_IMAGE_NULL "无法打开指定图像文件"
#define COLOUR_ERROR_FILE_NAME "文件后缀不是程序指定的特征库后缀，文件名错误"
#define COLOUR_ERROR_FILE_NOT_EXISTENCE	"该文件不存在"
#define COLOUR_ERROR__NOT_NO_METHOD	"没有这个方法"
#define COLOUR_ERROR_FILE_ERROR	"这不是一个正确的特征库文件"

//模糊方法
#define COLOUR_FUZZY_METHOD_GAUSSIAN 0	//高斯模糊
#define COLOUR_FUZZY_METHOD_SIMPLE 1	//简单模糊
#define COLOUR_FUZZY_METHOD_MEDIAN 2	//中值模糊
//colour sign
char colour_sign[] = { "<colour:红色,MIN:R=0&G=0&B=0&,MIX:R=255&G=0&B=0&><colour:蓝色,MIN:R=0&G=0&B=0&,MIX:R=0&G=255&B=0&><colour:黄色,MIN:R=0&G=0&B=0&,MIX:R=0&G=0&B=255&>" };	//自带的颜色特征库
char *p_colour_sign = colour_sign;
//自带的颜色特征库，防止泄漏已经采用封闭式的方法嵌入到库当中，非开源特征库，当被打包成dll时该特征库就会被一并打包到dll中，被编译成二进制，但是缺点是无法以热更新的方式来更新特征库，当有新的特征库时必须更换dll！优点：保密性更强！


//-
//颜色范围控制器
typedef struct colour{
	colour(int _Val){//构造函数，会在初始化时调用,如果用户需要显示初始化时，调用该函数！
		memset(colour_name, _Val, sizeof(colour_name));
		for (int i = 0; i < 3; ++i){
			colour_min_rgb[i] = _Val;
			colour_mix_rgb[i] = _Val;
		}
		colour_size = _Val;
		*colour_next = _Val;
		*colour_top = _Val;
	}
	char colour_name[256];
	double colour_mix_rgb[3];
	double colour_min_rgb[3];
	int colour_size = 0;
	colour *colour_next = NULL;
	colour *colour_top = NULL;
}colour;
//颜色范围控制器相关操作函数

//颜色范围控制器分配内存
COLOURIN(void) colour_malloc(colour** colour_malloc){
	//函数思维：使用二级指针方式间接为colour_malloc分配内存，避免使用EAX，EDX寄存器！提高运行速度。
	*colour_malloc = (colour*)malloc(sizeof(colour));	//二级指针解引用
	(*colour_malloc)->colour_next = NULL;
}

//获取颜色范围控制器大小-已被colour_size变量代替，此函数已废弃
COLOURIN(void) colour_get_size(colour** colour_, int *size){
	for (int i = 0;; ++i){
		*(colour_) = (*colour_)->colour_next;
		if (*(colour_) = NULL){
			*size = i;
			break;
		}
	}
}

//颜色特征库结构化，注意该函数必须在第一行调用，否则没有颜色特征库将无法进行操作
COLOURIN(void) colour_range_control(colour** colour_ = NULL, char **rgb_str = NULL, COLOUR_BOOL Self_Identification_Library = COLOUR_BOOL_TRUE/*是否使用自带特征库，如果此选项为TRUE，则rgb_str文件无效*/){
	//判断指针是否为空
	if (*colour_ == NULL){
		colour_error_type.colour_sign = COLOUR_BOOL_TRUE;
		strcpy_s(colour_error_type.error_type, COLOUR_ERROR_NULL);
		return;
	}
	//判断rgb_str是否为空
	if (*rgb_str == NULL){
		colour_error_type.colour_sign = COLOUR_BOOL_TRUE;
		strcpy_s(colour_error_type.error_type, COLOUR_ERROR_NULL);
		return;
	}
	//判断是否使用自带的颜色特征码
	if (Self_Identification_Library == COLOUR_BOOL_TRUE){
		//使用自带的颜色特征码
		//直接使用递归方式
		colour_range_control(colour_, &p_colour_sign, COLOUR_BOOL_FALSE);
		return;	//结束此次函数

	}
	//获取标识符大小
	char *sing_str = *rgb_str;
	char sing_char = 0;
	int sign_size = 0;
	int colour_sing_str_size = strlen(*rgb_str);
	//获取总共有多少个颜色特征
	for (int i = 0; i <= colour_sing_str_size; ++i){
		sing_char = sing_str[i];
		if (sing_char == 0x3E){
			sign_size += 1;
		}
	}
	//(*colour_)->colour_size = colour_sing_str_size;
	//创建颜色特征器
	int j = 0/*用于记录读取特征码到哪儿了*/, n = 0/*用于做name下标*/, s = 0/*用于标示读到哪儿了*/, t = 0/*用于读取minxRGB值*/, min = 0/*用于标示min下标*/, of = 0/*用于迭代循环总下标j下标在获取到断点字符时就会停止记录of会继续记录*/, mix = 0/*记录mix*/, c = 0/*用于读取mixRGB值*/;
	for (int i = 0; i < sign_size; ++i){
		colour *colour_1 = NULL;
		colour_malloc(&colour_1);
		colour_1->colour_top = colour_1;
		colour_1->colour_size = sign_size;	//链表存储数据结构的大小
		memset(colour_1->colour_name, 0, sizeof(colour_1->colour_name));
		/*优化思路：注意在底层方面，c语言编译器在编译过程要将字符转换成十六进制在转换成asccii码，然后再转成二进制，所以我们直接将其转换成十六进制易于提高编译速度
		: = 0x3A
		, = 0x2C
		= = 0x3D
		& = 0x26
		> = 0x3E
		*/
		//循环获取
		for (;; ++j){
			if (sing_str[j] == 0x3A){
				switch (s){
				case 0:{
						   for (int k = j + 1;; ++k, ++n){
							   if (sing_str[k] == 0x2C){
								   s += 1;//特征库的名字获取完毕该获取颜色取值范围了
								   n = 0;
								   break;
							   }
							   colour_1->colour_name[n] = sing_str[k];

						   }
				}
				case 1:{//获取MIN，RGB
						   of = j;
						   min = 0;
						   char w[4] = { 0 };
						   for (int u = 0; u < 3; ++u){
							   for (int y = of + 1;; ++y){
								   if (sing_str[y] == 0x3D){
									   for (int f = y;; ++f){
										   if (sing_str[f] == 0x26){
											   colour_1->colour_min_rgb[min] = atof(w);
											   min += 1;
											   t = 0;
											   of = f;
											   break;
										   }
										   w[t] = sing_str[f + 1];
										   if (t < 3){//校验是否到达rgb最高值
											   t = t + 1;
										   }

									   }
									   break;
								   }

							   }
						   }
				}
				}

			}
			if (sing_str[j] == 0x2C){//获取到“,”时代表已经到结尾了，所以可以直接获取mix的RGB值！
				char w[4] = { 0 };
				mix = 0;
				for (int i = of; sing_str[i] != 0x3E; ++i){
					if (sing_str[i] == 0x3D){
						for (int d = i;; ++d){
							if (sing_str[d] == 0x26){
								colour_1->colour_mix_rgb[mix] = atof(w);
								mix += 1;
								c = 0;
								of = d;
								break;
							}
							w[c] = sing_str[d + 1];
							if (c < 3){
								c += 1;
							}

						}
					}
				}
				j = of;//调整j下标
				s = 0;//将标识位清零

				colour_1->colour_next = (*colour_);
				(*colour_) = colour_1;

				break;
				

			}
		}

	}

}

//颜色特征库结构化_文件形式，注意该函数必须在第一行调用，否则没有颜色特征库接下来的操作将无法进行
COLOURIN(void) colour_range_control_file(colour** colour_ = NULL, char *colour_file = NULL, COLOUR_BOOL Self_Identification_Library = COLOUR_BOOL_TRUE/*是否使用自带特征库，如果此选项为TRUE，则rgb_str文件无效*/){
	//判断指针是否为空
	if (*colour_ == NULL){
		colour_error_type.colour_sign = COLOUR_BOOL_TRUE;
		strcpy_s(colour_error_type.error_type, COLOUR_ERROR_NULL);
		return;
	}
	//判断颜色特征库文件路径是否为空
	if (colour_file == NULL){
		colour_error_type.colour_sign = COLOUR_BOOL_TRUE;
		strcpy_s(colour_error_type.error_type, COLOUR_ERROR_NULL);
		return;
	}
	//判断特征库是否正确
	int str_len = strlen(colour_file);
	int colour_size = strlen(COLOURFILE);	//为了统一化管理申请一个宏代表文件名
	char a[] = COLOURFILE;	//比对临时变量
	int a_size = colour_size;
	a_size = a_size - 1;
	for (int i = str_len; i > str_len - colour_size; --i, --a_size){	//判断文件后缀是否正确，注意程序是严格根据文件后缀进行特征库提取的，所以文件后缀很重要
		if (a[a_size] != colour_file[i]){
			colour_error_type.colour_sign = COLOUR_BOOL_TRUE;
			strcpy_s(colour_error_type.error_type, COLOUR_ERROR_FILE_ERROR);
			return;
		}


	}
	//判断是否使用自带的颜色特征码
	if (Self_Identification_Library == COLOUR_BOOL_TRUE){
		//使用自带的颜色特征码
		//如果使用自带的特征库，就不需要去解析和数据结构化特征库文件，直接调用colour_range_control函数即可，因为自带的特征库文件是存在于库中的而非文件，在库中的特征库是以str字符串形式存储的！所以直接传参到colour_range_control中即可！
		colour_range_control(colour_, &p_colour_sign, COLOUR_BOOL_FALSE);
		return;	//结束此次函数

	}
	//获取标识符大小
	FILE *fp = fopen(colour_file, "r");
	if (fp == NULL){	//无法打开这个特征库文件
		colour_error_type.colour_sign = COLOUR_BOOL_TRUE;
		strcpy_s(colour_error_type.error_type, COLOUR_ERROR_FILE_NULL);
		return;
	}
	//将特征库文件里的颜色特征读取出来
	fseek(fp, 0, SEEK_END); //定位到文件末 
	int nFileLen = ftell(fp); //文件长度
	fseek(fp, 0, SEEK_SET);	//恢复到文件头，防止从文件尾读取数据
	char *sing_str = (char*)malloc(nFileLen);	//malloc分配大小，文件多大，就分配多大的堆内存
	memset(sing_str, 0, nFileLen);	//分配的内存值无法确定，将其清空为0，确保内容可靠性
	fread(sing_str, nFileLen, 1, fp);
	char sing_char = 0;
	int sign_size = 0;
	int colour_sing_str_size = strlen(colour_file);
	//获取总共有多少个颜色特征
	for (int i = 0; i <= colour_sing_str_size; ++i){
		sing_char = sing_str[i];
		if (sing_char == 0x3E){
			sign_size += 1;
		}
	}
	//(*colour_)->colour_size = colour_sing_str_size;
	//创建颜色特征器
	int j = 0/*用于记录读取特征码到哪儿了*/, n = 0/*用于做name下标*/, s = 0/*用于标示读到哪儿了*/, t = 0/*用于读取minxRGB值*/, min = 0/*用于标示min下标*/, of = 0/*用于迭代循环总下标j下标在获取到断点字符时就会停止记录of会继续记录*/, mix = 0/*记录mix*/, c = 0/*用于读取mixRGB值*/;
	for (int i = 0; i < sign_size; ++i){
		colour *colour_1 = NULL;
		colour_malloc(&colour_1);
		colour_1->colour_top = colour_1;
		colour_1->colour_size = sign_size;	//链表存储数据结构的大小
		memset(colour_1->colour_name, 0, sizeof(colour_1->colour_name));
		/*优化思路：注意在底层方面，c语言编译器在编译过程要将字符转换成十六进制在转换成asccii码，然后再转成二进制，所以我们直接将其转换成十六进制易于提高编译速度
		: = 0x3A
		, = 0x2C
		= = 0x3D
		& = 0x26
		> = 0x3E
		*/
		//循环获取
		for (;; ++j){
			if (sing_str[j] == 0x3A){
				switch (s){
				case 0:{
						   for (int k = j + 1;; ++k, ++n){
							   if (sing_str[k] == 0x2C){
								   s += 1;//特征库的名字获取完毕该获取颜色取值范围了
								   n = 0;
								   break;
							   }
							   colour_1->colour_name[n] = sing_str[k];

						   }
				}
				case 1:{//获取MIN，RGB
						   of = j;
						   min = 0;
						   char w[4] = { 0 };
						   for (int u = 0; u < 3; ++u){
							   for (int y = of + 1;; ++y){
								   if (sing_str[y] == 0x3D){
									   for (int f = y;; ++f){
										   if (sing_str[f] == 0x26){
											   colour_1->colour_min_rgb[min] = atof(w);
											   min += 1;
											   t = 0;
											   of = f;
											   break;
										   }
										   w[t] = sing_str[f + 1];
										   if (t < 3){//校验是否到达rgb最高值
											   t = t + 1;
										   }

									   }
									   break;
								   }

							   }
						   }
				}
				}

			}
			if (sing_str[j] == 0x2C){//获取到“,”时代表已经到结尾了，所以可以直接获取mix的RGB值！
				char w[4] = { 0 };
				mix = 0;
				for (int i = of; sing_str[i] != 0x3E; ++i){
					if (sing_str[i] == 0x3D){
						for (int d = i;; ++d){
							if (sing_str[d] == 0x26){
								colour_1->colour_mix_rgb[mix] = atof(w);
								mix += 1;
								c = 0;
								of = d;
								break;
							}
							w[c] = sing_str[d + 1];
							if (c < 3){
								c += 1;
							}

						}
					}
				}
				j = of;//调整j下标
				s = 0;//将标识位清零

				colour_1->colour_next = (*colour_);
				(*colour_) = colour_1;

				break;


			}
		}

	}

}
//颜色特征库释放
COLOURIN(void) colour_dele(colour** colour_){
	colour* colour_next = NULL;
	if ((*colour_)->colour_size == 0){
		PASS	//什么都不做
			return;
	}
	colour_next = (*colour_)->colour_next;
	free((*colour_));
	*colour_ = NULL;
	for (int i = 0; i < (*colour_)->colour_size; ++i){
		if (i == 0){//判断是否是第一次，释放首节点，注意如果一开始释放colour的话则无法获取后面的next节点地址，则造成内存泄漏
			colour_next = (*colour_)->colour_next;
			free((*colour_));
		}
		else{//colour_变量已经被释放了，但是我们已经指向了next节点，所以逐一释放首节点之后的节点即可！
			colour* lour = colour_next;
			colour_next = colour_next->colour_next;	
			free(lour);
		}
	}
}

//颜色范围控制器，手动训练生成器，面向用户的手动生成特征库,版本：1.0  该程序为颜色识别器接口自带的一个程序，可以直接调用
COLOURIN(void) colour_manual_generate(char colour_file_path[]/*colour** colour_ = NULL ,日后可能会增加其他功能会用到，先留一个接口*/){
	//参数值
	int colour_bit = 1;	//多通道
	COLOUR_BOOL open_file = COLOUR_BOOL_FALSE;
	COLOUR_BOOL file_name = COLOUR_BOOL_TRUE;
	/*
	if (*colour_ != NULL){
		colour_error_type.colour_sign = COLOUR_BOOL_TRUE;
		strcpy_s(colour_error_type.error_type, COLOUR_ERROR_NULL);
		return;
	}
	*/
	char l_w = 0;
	printf("*****************************\n");
	printf("图像颜色识别器-手动训练器\n");
	printf("*****************************\n");
	printf("请输入当前操作系统型号，以便于后面的环境初始化“L(linux)/W(windows)”：");
	scanf("%c", &l_w);
	getchar();	//把空格吃掉
	int kpt = -1;
	//0x6C = l 0x77 = w
	//0x4C = L 0x57 = W
	if (l_w == 0x6C || l_w == 0x4C){	//linux
		kpt = 0;
		printf("\n程序以Linux环境开始构建手动颜色训练库环境..\n\n");
	}
	else if (l_w == 0x77 || l_w == 0x57){  //windows
		kpt = 1;
		printf("\n程序以Windows环境开始构建手动颜色训练库环境..\n\n");
	}
	else{
		printf("\n输入的操作系统类型，程序无法识别，使用默认系统型号进行环境构造：Windows\n\n");
		kpt = 1;
	}
	ins:
	//----开始面向用户的手动生成特征库
	printf("正在初始化环境...\n\n");
	/*
	colour_ = {0};	//显示初始化结构体
	*/
	printf("显示初始化数据结构环境完成!\n\n");
	//----生成特征识别手动训练环境界面
	printf("*****************************\n\n");
	printf("图像颜色识别器-手动生成训练库1.0\n\n");
	printf("*****************************\n\n");
	printf("是否已有训练库？(Y/N)：");
	char y_n = 0;
	scanf("%c", &y_n);
	FILE *fp = NULL;
	//0x79 = y 0x6E = n
	//0x59 = Y 0x4E = N
	if (y_n == 0x79 || y_n == 0x59){	//y
		in_y:
		printf("\n请输入训练库位置:");
		char path[256] = {0};
		scanf("%s", path);
		getchar();	//吃掉回车
		int str_len = strlen(path);
		int colour_size = strlen(COLOURFILE);	//为了统一化管理申请一个宏代表文件名
		char a[] = COLOURFILE;	//比对临时变量
		int a_size = colour_size;
		a_size = a_size - 1;	//数组下标从0开始，所以要-1
		for (int i = str_len - 1; i > str_len - colour_size; --i, --a_size){	//判断文件后缀是否正确，注意程序是严格根据文件后缀进行特征库提取的，所以文件后缀很重要
			if (a[a_size] != path[i]){
				printf("\n这不是一个正确的特征库文件格式,请重新输入\n\n");
				goto in_y;
			}
		}
		fopen_s(&fp, path, "r");	//先以读的方式打开，确保文件存在
		if (fp == NULL){
			printf("无法打开该文件，文件不存在！\n\n请重新输入\n\n");
			goto in_y;
		}
		else{
			if (kpt == 0){
				system("clear");
			}
			if (kpt == 1){
				system("cls");
			}
			printf("*************************************\n");
			printf("正在加载%s特征库中....\n", path);
			fclose(fp);
			fopen_s(&fp, path, "a");	//以附加的方式打开
			printf("加载特征库:%s成功\n", path);
			printf("*************************************\n\n");
		}
	}
	else if (y_n == 0x6E || y_n == 0x4E){ //n
		if (kpt == 0){
			system("clear");
		}
		if (kpt == 1){
			system("cls");
		}
		//PASS;	//什么都不做
	}
	else{
		//跨平台特性，根据操作系统不同调用不同的终端处理程序，windows下清空终端缓冲流的程序是：cls，LINUX:clear
		if (kpt == 0){
			system("clear");
		}
		if (kpt == 1){
			system("cls");
		}
		printf("程序无法确定您输入的键代码是否正确，请在输入一次！\n\n");
		goto ins;
	}
	//开始进行手动颜色训练
	ins_1:
	printf("\n-请选择你要进行的手动训练规则：\n\n");
	printf("请根据编号选择对应要训练的方法：\n\n");
	printf("1.导入一张图片，由程序提取出图片中的颜色RGB分量，并将RGB分量告知用户，在由用户选择对应的RGB分量并将RGB分量对应的颜色名输入进程序,该过程会略过重复像素点！\n\n");
	printf("2.根据已知的RGB分量将颜色名输入到程序当中，并格式化保存到特征库中，特征库存在则附加不存在则新建！\n\n");
	printf("3.比对参数调整\n\n");
	printf("4.退出手动训练\n\n");
	printf("<请输入您的选择>:");
	int option = 0;
	scanf("%d", &option);
	getchar();	//吃掉回车
	int d = 0;
	colour *s = {0};
	colour_malloc(&s);
	FILE *RGB = NULL;
	char sd[1024] = {0};
	switch (option){
	case 1:{	//根据图像提取
			   COLOUR_BOOL image_ = COLOUR_BOOL_NOT;//二次判断
		   img_in:
			   if (kpt == 0){
				   system("clear");
			   }
			   if (kpt == 1){
				   system("cls");
			   }
			   if (image_ == COLOUR_BOOL_FALSE){
				   printf("图像文件无法打开.请重新输入\n");
			   }
			   printf("请输入要检索的图片文件路径：");
			   char img_path[256] = {0};
			   scanf("%s", &img_path);
			   getchar();	//吃掉回车fflus
			   if (kpt == 0){
				   system("clear");
			   }
			   if (kpt == 1){
				   system("cls");
			   }
			   printf("******************************\n");
			   printf("正在加载图像文件...\n");
			   IplImage *image = NULL;	//img
			   if (colour_bit == 1){
				   image = cvLoadImage(img_path);    //多通道
			   }
			   else{
				   image = cvLoadImage(img_path,0);	//单通道
			   }
			   if (image == NULL){
				   image_ = COLOUR_BOOL_FALSE;
				   goto img_in;
			   }
			   else{
				   image_ = COLOUR_BOOL_TRUE;
			   }
			   printf("加载图像文件成功\n");
			   printf("******************************\n");
			   //开始检索RGB颜色分量
			   if (fp != NULL){	//代表有特征库
				   //声明一个颜色选择器
				   colour* c = NULL;
				   //分配内存
				   colour_malloc(&c);
				   //将颜色特征库中的特征提取出来
				   fseek(fp, 0, SEEK_END); //定位到文件末 
				   int nFileLen = ftell(fp); //文件长度
				   char *colour_str = (char*)malloc(nFileLen);
				   fread(colour_str, nFileLen, 1, fp);	//颜色特征提取
				   colour_range_control(&c, &colour_str, COLOUR_BOOL_FALSE);	//颜色特征结构化
				   if (colour_error_type.colour_sign == COLOUR_BOOL_TRUE){	//判断有没有错误
					   printf("colour_type_error:%s,按下回车重新输入", colour_error_type.error_type);
					   getchar();
					   goto img_in;
				   }
				   //开始识别
				   //开始识别颜色
				   printf("请输入要将特征提取后的数据保存的路径：");
				   char path[256];
				   scanf("%c", path);
				   getchar();	//回车
				   if (kpt == 0){
					   system("clear");
				   }
				   if (kpt == 1){
					   system("cls");
				   }
				   printf("********************************\n");
				   printf("正在打开保存文件中...\n");
				   CvScalar scalar;    //scalar  
				   int num = 0;
				   int form_size = 0;
				   FILE* colour_fp = NULL;
				   unsigned long int colour_num = 0;
				   //先以自读方式打开，查看文件是否存在
				   fopen_s(&colour_fp, path, "r");
				   if (colour_fp == NULL){
					   printf("无法正常打开文件，文件不存在.\n");
					   printf("正在创建文件中...");
					   fopen_s(&colour_fp, path, "w+");
					   if (colour_fp == NULL){
						   printf("无法创建该文件!\n按下回车结束。");
						   getchar();
						   return;
					   }
					   else{
						   printf("文件创建成功!\n");
					   }

				   }
				   //附加方式打开
				   fclose(colour_fp);
				   fopen_s(&colour_fp, path, "a");
				   printf("********************************\n");
				   printf("开始读取,大约需要：%d次\n", image->height);
				   for (int i = 0; i <= image->height - 1; ++i){
					   printf("正在进行第%d次检索\n", i);
					   for (int j = 0; j <= image->width - 1; ++j){
						   scalar = cvGet2D(image, i, j);    //获取像素点的RGB颜色分量  
						   //比对算法_多通道
						   if (colour_bit == 1){
							   if (scalar.val[2] >= c->colour_min_rgb[0] && scalar.val[1] >= c->colour_min_rgb[1] && scalar.val[0] >= c->colour_min_rgb[2] &&
								   scalar.val[2] <= c->colour_mix_rgb[0] && scalar.val[1] <= c->colour_mix_rgb[1] && scalar.val[0] <= c->colour_mix_rgb[2]){	//如果颜色与特征库颜色一致，则直接pass
								   printf("发现重复的RGB颜色其特征为：MIN:%d,%d,%d ~ MIX:%d,%d,%d，所处像素点位置是:第%d行第%d列", c->colour_min_rgb[0], c->colour_min_rgb[1], c->colour_min_rgb[2], c->colour_mix_rgb[0], c->colour_mix_rgb[1], c->colour_mix_rgb[2], i, j);


							   }
							   else{
								   //否则保存到本地文件
								   char as[256];
								   sprintf(as, "R:%d,G:%d,B:%d\n", scalar.val[2], scalar.val[1], scalar.val[0]);
								   fwrite(as, strlen(as) + 1/*包含\n*/, 1, colour_fp);
								   colour_num += 1;
							   }
						   }//比对算法_单通道
						   else if (colour_bit == 0){
							   if (scalar.val[2] >= c->colour_min_rgb[0] && scalar.val[0] <= c->colour_mix_rgb[2]){	//如果颜色与特征库颜色一致，则直接pass
								   printf("发现重复的颜色其特征为：%d ~ %d，所处像素点位置是:第%d行第%d列", c->colour_min_rgb[0], c->colour_mix_rgb[0], i, j);
							   }
							   else{
								   //否则保存到本地文件
								   char as[256];
								   sprintf(as, "colour:%d\n", scalar.val[2], scalar.val[1], scalar.val[0]);
								   fwrite(as, strlen(as) + 1/*包含\n*/, 1, colour_fp);
								   colour_num += 1;
							   }
						   }
					   }
					   printf("检索完成，本次共获取：%d个未知RGB颜色分量！按下回车继续", colour_num);
					   if (open_file == COLOUR_BOOL_TRUE){
						   if (kpt == 0){	//跨平台
							   printf("已经为您自动打开特征库文件");
							   char system_[256] = { 0 };
							   sprintf(system_, "cat %s", colour_file_path);
							   system(system_);
						   }
						   if (kpt == 1){
							   printf("已经为您自动打开特征库文件");
							   char system_[256] = { 0 };
							   sprintf(system_, "cd %s", colour_file_path);
							   system(system_);
						   }

					   }

				   }
			   }
			   else{
				   printf("错误，没有为程序制定一个用于匹配的特征库，程序无法运行！\n\n");
				   printf("请重新输入您的特征库路径，在选择本功能!\n\n");
				   goto ins;

			   }
			   printf("检索完成.检索到的rgb颜色分量已经保存到：%s文件目录下，按下回车结束", colour_file_path);
			   getchar();
			   return;
	}
	break;
	case 2:{//根据已知RGB分量将颜色名输入到程序当中，并保存到特征库当中，附加，不存在则创建
			   fopen_s(&RGB, colour_file_path, "r");
			   if (RGB == NULL){//文件不存在,创建文件
				   fopen_s(&RGB, colour_file_path, "w");
			   }
			   //站着fp，防止防止被其他程序占用
			   if (kpt == 0){
				   system("clear");
			   }
			   if (kpt == 1){
				   system("cls");
			   }
			   int s1 = 0;
		   ins_2:
			   printf("**************************\n");
			   printf("-手动RGB颜色分量训练\n\n");
			   printf("**************************\n");
			   printf("请输入要获取的分量次数:");
			   scanf("%d", &d);
			   printf("\n");
			   if (d > 99){
				   printf("超出大小限制，一次只能输入99次\n\n");
				   goto ins_2;
			   }
			   char asd[1024];
			   getchar();	//回车
			   for (int i = 0; i < d; ++i){
				   printf("\n正在进行第“%d”次颜色特征获取\n\n",i+1);
				   printf("\n颜色名：");
				   scanf("%s", s->colour_name);
				   getchar();	//回车
				   printf("MIN-RGB（以“空格”分割）:");
				   scanf("%d %d %d", &s->colour_min_rgb[0], &s->colour_min_rgb[1], &s->colour_min_rgb[2]);
				   getchar();	//回车
				   printf("MIX-RGB（以“空格”分割）:");
				   scanf("%d %d %d", &s->colour_mix_rgb[0], &s->colour_mix_rgb[1], &s->colour_mix_rgb[2]);
				   getchar();//回车
				   sprintf(sd, "%s<colour:%s,MIN:R=%d&G=%d&B=%d&,MIX:R=%d&G=%d&B=%d&>", sd,s->colour_name, s->colour_min_rgb[0], s->colour_min_rgb[1], s->colour_min_rgb[2], s->colour_mix_rgb[0], s->colour_mix_rgb[1], s->colour_mix_rgb[2]);
			   }
			   //检索完成保存
			   printf("手动训练颜色特征检索完成，正在将颜色特征保存到%s特征库中..\n", colour_file_path);
			   //判断一下要保存的文件是不是一个正确的colour特征库文件
			   int str_len = strlen(colour_file_path);
			   int colour_size = strlen(COLOURFILE);	//为了统一化管理申请一个宏代表文件名
			   char a[] = COLOURFILE;	//比对临时变量
			   int a_size = colour_size;
			   a_size = a_size - 1;	//数组下标从0开始，所以要-1
			   for (int i = str_len-1; i > str_len - colour_size; --i, --a_size){	//判断文件后缀是否正确，注意程序是严格根据文件后缀进行特征库提取的，所以文件后缀很重要
				   if (a[a_size] != colour_file_path[i]){
					   printf("要保存的文件不是正确的特征库文件.\n");
					   if (file_name == COLOUR_BOOL_TRUE){
						   printf("正在重新规划特征库文件中..\n");
						   if (str_len >= FILE_NAME_MIX - a_size){	//arr size - colour_file_size = 247(-1),多减1是出于越界的安全的考虑，文件名大小超出限制
							   printf("无法重新规划特征库文件!\n");
							   colour_error_type.colour_sign = COLOUR_BOOL_TRUE;
							   strcpy_s(colour_error_type.error_type, COLOUR_ERROR_FILE_NAME);
							   return;
						   }
						   else{
							   //为文件自动加上后缀
							   char b[] = COLOURFILE;
							   int k = 0;
							   for (int j = str_len; j < str_len + colour_size; ++j, ++k){
								   colour_file_path[j] = b[k];
								   if (k == colour_size){	//谨防越界
									   break;
								   }
							   }
							   break;
						   }
					   }
				   }


			   }
			   FILE* fp = NULL;
			   fclose(RGB);
			   fopen_s(&fp, colour_file_path, "a");
			   if (fp == NULL){
				   colour_error_type.colour_sign = COLOUR_BOOL_TRUE;
				   strcpy_s(colour_error_type.error_type, COLOUR_ERROR_FILE_NULL);
				   return;
			   }
			   fwrite(sd, strlen(sd), 1, fp);
			   //手动训练特征库结束
			   printf("训练特征库结束，特征已保存到:%s目录文件下\n", colour_file_path);
			   if (open_file == COLOUR_BOOL_TRUE){
				   printf("已经为您自动打开特征库文件");
				   if (kpt == 0){	//跨平台
					   char system_[256] = { 0 };
					   sprintf(system_, "cat %s", colour_file_path);
					   system(system_);
				   }
				   if (kpt == 1){
					   char system_[256] = { 0 };
					   sprintf(system_, "cd %s", colour_file_path);
					   system(system_);
				   }

			   }
			   printf("按下回车结束，并将缓存数据写入到文件当中!");
			   getchar();
			   
	}
	break;
	case 3:{//比对参数调整
			   if (kpt == 0){
				   system("clear");
			   }
			   if (kpt == 1){
				   system("cls");
			   }
			   printf("\n请选择要设置的参数:\n\n");
			   printf("1.设置图像通道\n\n");
			   printf("2.设置检索完打开与关闭\n\n");
			   printf("3.设置存储文件格式不正确是否自动订正\n\n");
			   printf("4.设置对比算法\n\n");
			   printf("5.返回上一级选项\n\n");
			   printf(":");
			   char a;
			   scanf("%d", &a);
			   switch (a){
			   case 1:
				   printf("请输入多通道还是单通道0(单通道)/1(多通道)：");
				   int in;
				   scanf("%d", &in);
				   if (in == 0){
					   if (kpt == 0){
						   system("clear");
					   }
					   if (kpt == 1){
						   system("cls");
					   }
					   printf("设置完成，当前以单通道模式检索图像\n");
					   goto ins_1;
				   }
				   else if (in == 1){
					   if (kpt == 0){
						   system("clear");
					   }
					   if (kpt == 1){
						   system("cls");
					   }
					   printf("设置完成，当前以多通道模式检索图像\n");
					   goto ins_1;
				   }
				   else{
					   if (kpt == 0){
						   system("clear");
					   }
					   if (kpt == 1){
						   system("cls");
					   }
					   printf("输入有误..无法完成当前参数设置请求");
					   goto ins_1;
				   }
				   break;
			   case 2:
				   ins_3:
				   printf("手动训练完成后是否自动打开特征库文件?(y/n)：");
				   char a;
				   scanf("%c", &a);
				   if (a == 0x79 || a == 0x59){	//y
					   open_file = COLOUR_BOOL_TRUE;
					   if (kpt == 0){
						   system("clear");
					   }
					   if (kpt == 1){
						   system("cls");
					   }
					   printf("设置完成，当训练完成颜色特征后，会自动将存储颜色特征的特征库文件打开！\n");
					  
				   }
				   else if (a == 0x6E || a == 0x4E){ //n
					   open_file = COLOUR_BOOL_FALSE;
					   if (kpt == 0){
						   system("clear");
					   }
					   if (kpt == 1){
						   system("cls");
					   }
					   printf("设置完成，当训练完成颜色特征后，不会自动将存储颜色特征的特征库文件打开！\n");
				   }
				   else{
					   if (kpt == 0){
						   system("clear");
					   }
					   if (kpt == 1){
						   system("cls");
					   }
					   printf("输入有误..无法完成当前参数设置请求\n");
					   goto ins_3;
				   }
				   break;
			   case 3:
				   printf("是否在文件存储格式不正确时自动订正？0(yes)/1(no)：");
				   int in1;
				   scanf("%d", &in1);
				   if (in1 == 0){
					   file_name = COLOUR_BOOL_TRUE;
					   if (kpt == 0){
						   system("clear");
					   }
					   if (kpt == 1){
						   system("cls");
					   }
					   printf("设置完成，当文件格式不正确时会自动订正\n");
					   goto ins_1;
				   }
				   else if (in1 == 1){
					   file_name = COLOUR_BOOL_FALSE;
					   if (kpt == 0){
						   system("clear");
					   }
					   if (kpt == 1){
						   system("cls");
					   }
					   printf("设置完成，当文件格式不正确时不会自动订正，会主动提示用户\n");
					   goto ins_1;
				   }
				   else{
					   if (kpt == 0){
						   system("clear");
					   }
					   if (kpt == 1){
						   system("cls");
					   }
					   printf("输入有误..无法完成当前参数设置请求");
					   goto ins_1;
				   }
				   break;
			   case 4:
				   if (kpt == 0){
					   system("clear");
				   }
				   if (kpt == 1){
					   system("cls");
				   }
				   printf("暂时仅只有传统像素颜色值对比一种算法！\n\n");
				   goto ins_1;
				   break;
			   case 5:
				   if (kpt == 0){
					   system("clear");
				   }
				   if (kpt == 1){
					   system("cls");
				   }
				   goto ins_1;
				   break;
			   default:
				   if (kpt == 0){
					   system("clear");
				   }
				   if (kpt == 1){
					   system("cls");
				   }
				   printf("错误的选项\n");
				   goto ins_1;

			   }
	}
	break;
	case 4:{	//退出
			   printf("\n图像识别器-手动训练1.0 进程已经结束\n");
			   return;
	}
	break;
	default:
		printf("错误的选项\n");
		goto ins;

	}

	
}


//自动训练器1.0
COLOURIN(void) machine_learning_colour_train(char path[]/*图片文件路径*/, char txt[]/*训练文件集*/, char Catalog[]/*训练结果目录*/,char colour_path[]/*特征库路径*/){
	char path_txt[1024] = {0};
	sprintf(path_txt, "%s\\%s", path, txt);
	FILE *fp = fopen(path_txt, "r");
	if (fp == NULL){
		printf("无法开始训练，原因：无法打开训练文件集");
		getchar();
		return;
	}
	//获取训练好特征库存放路径
	char path_Catalog[1024] = { 0 };
	sprintf(path_Catalog, "%s\\%s", Catalog, KEY_FILE);
	FILE *fp1 = fopen(path_Catalog, "r");	//先以只读的方式确认文件是否存在
	if (fp1 == NULL){	//不存在创建
		fp1 = fopen(path_Catalog, "w");
	}
	else{	//存在以附加的方式打开
		fclose(fp1);	//关闭重新打开
		fp1 = fopen(path_Catalog, "a");
	}
	char colour_str[256] = {0};	//训练特征临时存放站点
	int error_num = 0;	//记录训练失败的图像文件	
	int ok_num = 0;	//记录训练成功的图像文件
	//特征库文件数据结构化
	colour* colour_ = NULL;
	colour_malloc(&colour_);
	colour_range_control(&colour_);
	if (colour_error_type.colour_sign == COLOUR_BOOL_TRUE){
		printf("colour_type_error:%s", colour_error_type.error_type);
		getchar();
		return;
	}
	if (fp == NULL){
		printf("无法完成自动训练,配置文件不存在!\n");
	}
	printf("*********************开始训练*********************\n");
	char strLine[256] = { 0 };
	int i = 0;	//训练记录器
	int num = 0;	//已经检索多少像素点
	int file_size = 0;	//有多少文件
	while (!feof(fp))                                   //循环读取每一行，直到文件尾  
	{
		fgets(strLine, sizeof(strLine), fp);
		file_size += 1;	//++file_num
	}
	//重新挪移指针,定位到文件头
	fseek(fp, 0L, SEEK_SET);
	while (!feof(fp))                                   //循环读取每一行，直到文件尾  
	{
		fgets(strLine, sizeof(strLine), fp);                     //将fp所指向的文件一行内容读到strLine缓冲区  
		if (strLine[strlen(strLine) - 1/*arr_0*/] == '\n'){	//删除换行
			strncpy(strLine, strLine, (strlen(strLine) - 1));	//copy原数据-1(\n)

		}
		//已经得到第一个文件名开始循环训练
		i += 1;
		printf("正在训练第“%d/%d”张图片", i+1,file_size+1);
		//开始训练第x张图片
		IplImage *image = cvLoadImage(strLine,1);	//原图读入第一个图像文件
		if (image == NULL){
			printf("%s特征图像文件无法打开..\n", strLine);
			error_num += 1;
		}
		else{
			CvScalar scalar;    //scalar  
			for (int i = 0; i <= image->height - 1; ++i){
				for (int j = 0; j <= image->width - 1; ++j){
					scalar = cvGet2D(image, i, j);    //获取像素点的RGB颜色分量  
					for (int z = 0;; ++z){
						//比对算法_1
						if (scalar.val[2] >= colour_->colour_min_rgb[0] && scalar.val[1] >= colour_->colour_min_rgb[1] && scalar.val[0] >= colour_->colour_min_rgb[2] &&
							scalar.val[2] <= colour_->colour_mix_rgb[0] && scalar.val[1] <= colour_->colour_mix_rgb[1] && scalar.val[0] <= colour_->colour_mix_rgb[2]){

							//过滤掉重复颜色，下一种颜色
							if (num < colour_->colour_size){	//略过无用循环，当颜色特征库用完时代表无法判断出更多的颜色，直接返回已获取到的颜色
								colour_ = colour_->colour_next;
								num += 1;
							}
							else{
								for (int y = 0; y < num; ++y){
									colour_ = colour_->colour_top;
								}
								num = 0; 
								break;	//跳出循环
							}

						}
						else{	//将特征库里没有识别出来的颜色保存起来
							memset(colour_str, 0, sizeof(colour_str));
							sprintf(colour_str, "<colour:,MIN:R=%d&G=%d&B=%d&,MIX:R=%d&G=%d&B=%d&>", 0, 0, 0, scalar.val[2], scalar.val[1], scalar.val[0]);
							fwrite(path_Catalog, strlen(path_Catalog), 1, fp1);

						}
					}
				}
			}
		}
		printf("开始训练下一张\n");
		
	}
}
//将一副RGB图像的所有颜色信息保存到本地
COLOURIN(void) machine_learning_colour_train_debug(IplImage *To_Identify_Colour_The_Image/*图片文件路径*/, char path[]/*信息存放路径*/){
	IplImage *imag = cvLoadImage(path);
	CvScalar scalar;    //scalar  

	if (imag == NULL){
		colour_error_type.colour_sign = COLOUR_BOOL_TRUE;
		strcpy_s(colour_error_type.error_type, COLOUR_ERROR_FILE_IMAGE_NULL);
		return;
	}
	FILE *fp = fopen(path, "w");
	if (fp == NULL){ 
		colour_error_type.colour_sign = COLOUR_BOOL_TRUE;
		strcpy_s(colour_error_type.error_type, COLOUR_ERROR_FILE_NULL);
		return;
	}
	char str[256] = { 0 };
	for (int i = 0; i <= imag->height - 1; ++i){
		for (int j = 0; j <= imag->width - 1; ++j){
			//将数据写入
			memset(str, 0, sizeof(str));
			sprintf(str, "height_inx:%d,width_inx:%d,R=%d,G=%d,B=%d\n", i, j, scalar.val[2], scalar.val[1], scalar.val[0]);
			fwrite(str, strlen(str), 1, fp);
		}
	}
	
}
//将已有的颜色特征保存到特征库文件中
COLOURIN(void) colour_features_seve(char str[],char path[256]){
	//判断一下要保存的文件是不是一个正确的colour特征库文件
	int str_len = strlen(path);
	int colour_size = strlen(COLOURFILE);	//为了统一化管理申请一个宏代表文件名
	char a[] = COLOURFILE;	//比对临时变量
	int a_size = colour_size;
	a_size = a_size - 1;
	for (int i = str_len; i > str_len - colour_size; --i, --a_size){	//判断文件后缀是否正确，注意程序是严格根据文件后缀进行特征库提取的，所以文件后缀很重要
		if (a[a_size] != path[i]){
			if (str_len >= FILE_NAME_MIX - a_size){	//arr size - colour_file_size = 249(-1),多减1是出于越界的安全的考虑
				colour_error_type.colour_sign = COLOUR_BOOL_TRUE;
				strcpy_s(colour_error_type.error_type, COLOUR_ERROR_FILE_NAME);
				return;
			}
			else{
				//为文件自动加上后缀
				char b[] = COLOURFILE;
				int k = 0;
				for (int j = str_len - 1;j < str_len+colour_size - 1; ++j,++k){
					path[j] = b[k];
					if (k == colour_size - 1){
						break;
					}
				}
				break;
			}
		}
	
	
	}
	//写入文件
	FILE* fp = NULL;
	fopen_s(&fp,path, "w");
	if (fp == NULL){
		colour_error_type.colour_sign = COLOUR_BOOL_TRUE;
		strcpy_s(colour_error_type.error_type, COLOUR_ERROR_FILE_NULL);
		return;
	}
	fwrite(str, strlen(str), 1, fp);

}

//-
//图像颜色描述器
typedef struct image_colour_form{
	char colour_name[256];
	int colour_num = 0;
	image_colour_form* next_colour_name;
	image_colour_form* top_colour_name;

}image_colour_form;
//图像颜色描述器相关操作函数

//颜色描述器分配内存
COLOURIN(void) image_form_malloc(image_colour_form** image_colour_form_malloc){
	//函数思维二级指针方式间接为image_colour_form_malloc分配内存，避免使用EAX，EDX寄存器！提高运行速度。
	*image_colour_form_malloc = (image_colour_form*)malloc(sizeof(image_colour_form));	//二级指针解引用
}

//颜色特征库加载
COLOURIN(char *)colour_sing_load(char path[]){
	//判断一下要加载的文件是不是一个正确的colour特征库文件
	int str_len = strlen(path);
	int colour_size = strlen(COLOURFILE);	//为了统一化管理申请一个宏代表文件名
	char a[] = COLOURFILE;	//比对临时变量
	int a_size = colour_size;
	a_size = a_size - 1;
	for (int i = str_len; i > str_len - colour_size; --i, --a_size){	//判断文件后缀是否正确，注意程序是严格根据文件后缀进行特征库提取的，所以文件后缀很重要
		if (a[a_size] != path[i]){
			if (str_len >= FILE_NAME_MIX - a_size){	//arr size - colour_file_size = 249(-1),多减1是出于越界的安全的考虑
				colour_error_type.colour_sign = COLOUR_BOOL_TRUE;
				strcpy_s(colour_error_type.error_type, COLOUR_ERROR_FILE_NAME);
				return "-1";
			}
			else{
				//为文件自动加上后缀
				char b[] = COLOURFILE;
				int k = 0;
				for (int j = str_len - 1; j < str_len + colour_size - 1; ++j, ++k){
					path[j] = b[k];
					if (k == colour_size - 1){
						break;
					}
				}
				break;
			}
		}


	}
	FILE* fp = NULL;
	fopen_s(&fp, path, "w");
	if (fp == NULL){
		colour_error_type.colour_sign = COLOUR_BOOL_TRUE;
		strcpy_s(colour_error_type.error_type, COLOUR_ERROR_FILE_NULL);
		return "-1";
	}
	fseek(fp, 0, SEEK_END); //定位到文件末 
	int nFileLen = ftell(fp); //文件长度
	fseek(fp, 0, SEEK_SET);	//恢复到文件头，防止从文件尾读取数据
	char *colour_str = (char*)malloc(nFileLen);
	memset(colour_str, 0, nFileLen);	//分配的内存值无法确定，将其清空为0，确保内容可靠性
	fread(colour_str, nFileLen, 1, fp);
	return colour_str;
}

//颜色识别函数
image_colour_form *Colour_Distinguish(IplImage *To_Identify_Colour_The_Image, colour** colour_)        {
	image_colour_form *image_colour_form_return1;
	image_form_malloc(&image_colour_form_return1);
	//开始识别颜色
	CvScalar scalar;    //scalar  
	int num = 0;
	int form_size = 0;
	int colour_num = 0;
	for (int i = 0; i <= To_Identify_Colour_The_Image->height - 1; ++i){	//-1是因为堆栈sp指针指向地址下标是从0开始的
		for (int j = 0; j <= To_Identify_Colour_The_Image->width - 1; ++j){
			scalar = cvGet2D(To_Identify_Colour_The_Image, i, j);    //获取像素点的RGB颜色分量  
			for (int k = 0;k<(*colour_)->colour_size; ++k){
				//比对算法_1
				//<= <,>=,>  在对比时必须确保范围控制在min到mix之间，所以要保证所有可能性比如大于等于或大于，小于等于或小于
				if ((scalar.val[2] >= (*colour_)->colour_min_rgb[0] && scalar.val[1] >= (*colour_)->colour_min_rgb[1] && scalar.val[0] >= (*colour_)->colour_min_rgb[2])||
					(scalar.val[2] > (*colour_)->colour_min_rgb[0] && scalar.val[1] > (*colour_)->colour_min_rgb[1] && scalar.val[0] > (*colour_)->colour_min_rgb[2])
					&&(scalar.val[2] <= (*colour_)->colour_mix_rgb[0] && scalar.val[1] <= (*colour_)->colour_mix_rgb[1] && scalar.val[0] <= (*colour_)->colour_mix_rgb[2])||
					(scalar.val[2] < (*colour_)->colour_mix_rgb[0] && scalar.val[1] < (*colour_)->colour_mix_rgb[1] && scalar.val[0] < (*colour_)->colour_mix_rgb[2])){
					image_colour_form *image_colour_form_return;
					image_form_malloc(&image_colour_form_return);
					colour_num += 1;	//记录获取到了多少颜色
					strcpy_s(image_colour_form_return->colour_name, (*colour_)->colour_name); //将颜色名称读取出来
					image_colour_form_return->top_colour_name = image_colour_form_return;
					image_colour_form_return->next_colour_name = image_colour_form_return1;
					image_colour_form_return1 = image_colour_form_return;
					//过滤掉重复颜色，下一种颜色
					if (num < (*colour_)->colour_size){	//略过无用循环，当颜色特征库用完时代表无法判断出更多的颜色，开始获取下一个颜色值
						(*colour_) = (*colour_)->colour_next;
						num += 1;
					}
					else{
						for (int y = 0; y < num; ++y){//颜色轮回
							(*colour_) = (*colour_)->colour_top;
						}
						num = 0;
						break;
						//return image_colour_form_return;
					}
				}
				
			}
		}
	}
	for (int i = 0; i <= (*colour_)->colour_size; ++i){	//结点复原
		(*colour_) = (*colour_)->colour_top;
	}
	for (int i = 0; i <= image_colour_form_return1->colour_num; ++i){	//结点复原
		image_colour_form_return1->colour_num = colour_num;	//让每个节点都记录颜色总数
		image_colour_form_return1 = image_colour_form_return1->top_colour_name;
	}
	if (image_colour_form_return1->colour_num == 0){
		sprintf(image_colour_form_return1->colour_name, "NULL");
	}
	return image_colour_form_return1;

}

//颜色识别函数-调试版
image_colour_form * Colour_Distinguish_Debug(IplImage *To_Identify_Colour_The_Image, colour** colour_,char colour_error_path[]){
	
	image_colour_form *image_colour_form_return;
	image_form_malloc(&image_colour_form_return);
	image_colour_form *image_colour_form_return1;
	image_form_malloc(&image_colour_form_return1);
	//开始识别颜色
	CvScalar scalar;    //scalar  
	int num = 0;
	int form_size = 0;
	int colour_num = 0;
	FILE *fp = NULL;
	char str[256] = { 0 };
	fopen_s(&fp, colour_error_path, "w");
	if (fp == NULL){
		colour_error_type.colour_sign = COLOUR_BOOL_TRUE;
		strcpy_s(colour_error_type.error_type, COLOUR_ERROR_FILE_NULL);
		return (image_colour_form*)-1;
	}
	for (int i = 0; i <= To_Identify_Colour_The_Image->height - 1; ++i){
		for (int j = 0; j <= To_Identify_Colour_The_Image->width - 1; ++j){
			scalar = cvGet2D(To_Identify_Colour_The_Image, i, j);    //获取像素点的RGB颜色分量  
			for (int k = 0;; ++k){
				//比对算法_1
				if ((scalar.val[2] >= (*colour_)->colour_min_rgb[0] && scalar.val[1] >= (*colour_)->colour_min_rgb[1] && scalar.val[0] >= (*colour_)->colour_min_rgb[2]) ||
					(scalar.val[2] > (*colour_)->colour_min_rgb[0] && scalar.val[1] > (*colour_)->colour_min_rgb[1] && scalar.val[0] > (*colour_)->colour_min_rgb[2])
					&& (scalar.val[2] <= (*colour_)->colour_mix_rgb[0] && scalar.val[1] <= (*colour_)->colour_mix_rgb[1] && scalar.val[0] <= (*colour_)->colour_mix_rgb[2]) ||
					(scalar.val[2] < (*colour_)->colour_mix_rgb[0] && scalar.val[1] < (*colour_)->colour_mix_rgb[1] && scalar.val[0] < (*colour_)->colour_mix_rgb[2])){
					image_colour_form *image_colour_form_return;
					image_form_malloc(&image_colour_form_return);
					strcpy_s(image_colour_form_return->colour_name, (*colour_)->colour_name); //将颜色名称读取出来
					colour_num  += 1;
					image_colour_form_return->top_colour_name = image_colour_form_return;
					image_colour_form_return->next_colour_name = image_colour_form_return1;
					image_colour_form_return1 = image_colour_form_return;
					//过滤掉重复颜色，下一种颜色
					if (num < (*colour_)->colour_size){	//略过无用循环，当颜色特征库用完时代表无法判断出更多的颜色，直接返回已获取到的颜色
						(*colour_) = (*colour_)->colour_next;
						num += 1;
					}
					else{
						for (int y = 0; y < num; ++y){//颜色轮回
							(*colour_) = (*colour_)->colour_top;
						}
						num = 0;
						break;
						//return image_colour_form_return;
					}
				}
				else{	//无法识别的颜色,写入到文件当中
					sprintf(str, "error_colour:R = %d,G = %d,B = %d\n", scalar.val[2], scalar.val[1], scalar.val[0]);
					fwrite(str, strlen(str), 1, fp);

				}

			}
		}
	}
	for (int i = 0; i <= (*colour_)->colour_size; ++i){	//结点复原
		(*colour_) = (*colour_)->colour_top;
	}
	for (int i = 0; i <= image_colour_form_return1->colour_num; ++i){	//结点复原
		image_colour_form_return1->colour_num = colour_num;	//让每个节点都记录颜色总数
		image_colour_form_return1 = image_colour_form_return1->top_colour_name;
	}
	if (image_colour_form_return1->colour_num == 0){
		sprintf(image_colour_form_return1->colour_name, "NULL");
	}
	return image_colour_form_return1;
}


//-
//colour文件编辑器 1.0
COLOURIN(void) colour_vim(char colour_path[]){
	printf("***********************\n");
	printf("COLOUR特征库编辑器-1.0\n");
	printf("***********************\n");
	printf("正在加载特征库...");
	int str_len = strlen(colour_path);
	int colour_size = strlen(COLOURFILE);	//为了统一化管理申请一个宏代表文件名
	char a[] = COLOURFILE;	//比对临时变量
	int a_size = colour_size;
	a_size = a_size - 1;	//数组下标从0开始，所以要-1
	for (int i = str_len - 1; i > str_len - colour_size; --i, --a_size){	//判断文件后缀是否正确，注意程序是严格根据文件后缀进行特征库提取的，所以文件后缀很重要
		if (a[a_size] != colour_path[i]){
			printf("这不是一个正确的特征库文件.进程已缓存...");
			getchar();
			return;
		}
	}
	printf("文件格式校验成功\n");
	//以内存映射的方式管理文件
	//该方法不适合跨平台已舍弃
	/*
	HANDLE hFile = CreateFileForMapping(pathName,GENERIC_READ,NULL,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile == NULL)
	{
		DWORD it = GetLastError();//ERROR_ALREADY_EXISTS
		TRACE(_T("创建文件映射对象失败,错误代码:%d/r/n"), GetLastError());
		return;
	}
	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hFile == NULL)
	{
	DWORD it = GetLastError();//ERROR_ALREADY_EXISTS
	TRACE(_T("创建文件映射对象失败,错误代码:%d/r/n"), GetLastError());
	return;
	}
	*/
	//正统标准c模拟内存映射方法

	FILE* fp = fopen(colour_path, "r");	
	if (fp == NULL){
		printf("无法打开指定特征库文件，进程已缓存...");
		getchar();
		return;
	}
	printf("特征库打开成功\n");
	//文件打开成功模拟独立内存映射(非共享内存)
	fseek(fp,0, SEEK_END); //定位到文件末 
	int nFileLen = ftell(fp); //文件长度
	//获取特征库名
	char ad;
	char file_name[256] = { 0 };
	int min = 0, mix = 0;
	for (int i = strlen(colour_path);i>=0; --i){
			ad = colour_path[i];
			if (a == "."){//获取文件名最大值
			//分割线分割
				mix = i;

			}
			if(a == "\\"){//获取文件名最小值
				min = i;
				for (int j = 0; j <= mix; ++j,++min){
					file_name[j] = colour_path[min];
				}
				break;
			
			}

	}
	printf("特征库描述信息获取成功...\n");
	/*
	//防止无内容的空映射
	if (nFileLen < 10){
		printf("该特征库不正确，进程已经缓存...");
		getchar();
		return;
	}
	*/
	char *colour_str = (char*)malloc(nFileLen);
	fread(colour_str, nFileLen, 1,fp);	//文件内容到内存
	//结束文件内核I/O占用
	fclose(fp);
	printf("特征库映射完成\n");
	printf("正在进行特征库数据结构化..\n");
	//特征库数据结构化
	colour* colour_;
	colour_malloc(&colour_);
	colour_range_control(&colour_, &colour_str,COLOUR_BOOL_FALSE);
	if (colour_error_type.colour_sign == COLOUR_BOOL_TRUE){
		printf("无法完成数据结构特征化,内部错误,进程已缓存...");
		getchar();
		return;
	}
	//释放临时映射内存
	free(colour_str);
	colour_str = NULL;
	//数据结构化完成即可对其进行增删改操作
	//打印特征库信息
	printf("特征库数据结构化完成\n");
	printf("*********************************\n");
	printf("特征库描述信息:\n");
	printf("特征库文件名:%s\n", file_name);
	printf("特征库字节大小:%d\n", nFileLen);
	printf("特征库特征样本数量:%d\n", colour_->colour_size);
	printf("*********************************\n");
	ins:
	printf("请问您要对特征文件进行哪些操作？\n");
	printf("1.列出所有特征库样本信息\n");
	printf("2.查看指定特征库样本信息\n");
	printf("3.修改制定特征库样本信息\n");
	printf("4.删除制定特征库样本\n");
	printf("5.删除所有特征库样本\n");
	printf("6.保存操作\n");
	printf("7.保存成新的特征库文件\n");
	int lef = 0;
	int num = 0;
	int i = 0;
	char path_[256] = {0};
	switch (lef){
	case 1:{	//列出所有特征库样本信息
			   for (i = 0; i <= colour_->colour_size; ++i){
				   printf("%d.颜色名称：%s\nRGB最小上限值(MIN):R=%d,G=%d,B=%d\nRGB最大上限值(MIX):R=%d,G=%d,B=%d\n", i + 1, colour_->colour_name, colour_->colour_min_rgb[0], colour_->colour_min_rgb[1], colour_->colour_min_rgb[2], colour_->colour_mix_rgb[0], colour_->colour_mix_rgb[1], colour_->colour_mix_rgb[2]);
				   colour_ = colour_->colour_next;
			   }
			   printf("已经为您列出：%d个颜色样本\n", colour_->colour_size);
			   for (int j = 0; j <= i; ++j){
				   colour_ = colour_->colour_top;
			   }
			   goto ins;
	}
	case 2:{//查看指定特征库样本信息
			   ins_1:
			   printf("请输入要查看的样本编号:");
			   scanf("%d", &num);
			   if (num > colour_->colour_size || num < colour_->colour_size){
				   printf("编号大于颜色样本数量，或小于有效下标.请重新输入\n");
				   goto ins_1;
			   }
			   for (int i = 0; i <= colour_->colour_size; ++i){
				   if (i == num - 1){
					   printf("%d.颜色名称：%s\nRGB最小上限值(MIN):R=%d,G=%d,B=%d\nRGB最大上限值(MIX):R=%d,G=%d,B=%d\n", i + 1, colour_->colour_name, colour_->colour_min_rgb[0], colour_->colour_min_rgb[1], colour_->colour_min_rgb[2], colour_->colour_mix_rgb[0], colour_->colour_mix_rgb[1], colour_->colour_mix_rgb[2]);
					   for (int j = 0; j < i;++j){
						   colour_ = colour_->colour_top;
					   }
					   goto ins;
				   }
				   colour_ = colour_->colour_next;
				   
			   }
			   goto ins;

		}
	case 3:{	//修改指定特征库样本信息
			   ins_3:
			   printf("请输入指定特征样本编号：");
			   scanf("%d", &num);
			   if (num > colour_->colour_size || num < colour_->colour_size){
				   printf("编号大于颜色样本数量，或小于有效下标.请重新输入\n");
				   goto ins_3;
			   }
			   for (int i = 0; i <= colour_->colour_size; ++i){
				   if (i == num - 1){
					   printf("%d.颜色名称：%s\nRGB最小上限值(MIN):R=%d,G=%d,B=%d\nRGB最大上限值(MIX):R=%d,G=%d,B=%d\n", i + 1, colour_->colour_name, colour_->colour_min_rgb[0], colour_->colour_min_rgb[1], colour_->colour_min_rgb[2], colour_->colour_mix_rgb[0], colour_->colour_mix_rgb[1], colour_->colour_mix_rgb[2]);
					   printf("新的颜色名称：");
					   scanf("%s", colour_->colour_name);
					   getchar();	//回车
					   printf("新的RGB值最小值(MIN)以“空格”分隔：");
					   scanf("%d %d %d", colour_->colour_min_rgb[0], colour_->colour_min_rgb[1], colour_->colour_min_rgb[2]);
					   getchar();
					   printf("新的RGB值最大值(MIX)以“空格”分隔：");
					   scanf("%d %d %d", colour_->colour_mix_rgb[0], colour_->colour_mix_rgb[1], colour_->colour_mix_rgb[2]);
					   getchar();	//回车
					   printf("修改完成\n");
					   for (int j = 0; j <= i; ++j){
						   colour_ = colour_->colour_top;
					   }
					   goto ins;
				   }
				   colour_ = colour_->colour_next;
			}

	}
	case 4:{	//删除指定特征库样本信息
		   ins_4:
			   printf("请输入指定特征样本编号：");
			   scanf("%d", &num);
			   if (num > colour_->colour_size || num < colour_->colour_size){
				   printf("编号大于颜色样本数量，或小于有效下标.请重新输入\n");
				   goto ins_4;
			   }
			   for (int i = 0; i <= colour_->colour_size; ++i){
				   if (i == num - 1){
					   *colour_->colour_name = NULL;
					   for (int j = 0; j < 3; ++j){
						   colour_->colour_min_rgb[j] = 0;
						   colour_->colour_mix_rgb[j] = 0;
					   }
					   printf("删除完成\n");
					   for (int j = 0; j < i; ++j){
						   colour_ = colour_->colour_top;
					   }
					   goto ins;
				   }
				   colour_ = colour_->colour_next;
			   }
	}
	case 5:{	//删除所有特征库样本
			   printf("正在删除...\n");
			   colour_dele(&colour_);
			   printf("删除完成\n");
			   goto ins;
	
	}
	case 6:{	//保存操作
			   printf("正在映射到文件中...");
			   fp = fopen(colour_path, "w");
			   if (fp == NULL){
				   printf("无法保存到原文件中..\n原因：原文件已经失效..");
				   goto ins;
			   }
			   colour_str = (char*)malloc(nFileLen);
			   memset(colour_str, 0, nFileLen);
			   for (int i = 0; i <= colour_->colour_size; ++i){
				   colour_ = colour_->colour_next;
				   sprintf(colour_str, "%s<colour:%s,MIN:R=%d&G=%d&B=%d&,MIX:R=%d&G=%d&B=%d&>", colour_str,colour_->colour_name, colour_->colour_min_rgb[0], colour_->colour_min_rgb[1], colour_->colour_min_rgb[2], colour_->colour_mix_rgb[0], colour_->colour_mix_rgb[1], colour_->colour_mix_rgb[2]);
			   }
			   for (int j = 0; j < colour_->colour_size; ++j){	//恢复结点
				   colour_ = colour_->colour_top;
			   }
			   fwrite(colour_str, strlen(colour_str), 1, fp);	//写入文件
			   printf("映射文件成功");
			   fclose(fp);
			   free(colour_str);
			   colour_str = NULL;
			   goto ins;
		}
	case 7:{	//保存成新的特征文件
			   printf("请输入要保存的文件路径：");
			   scanf("%s", path_);
			   //保存前检查一下磁盘是否能正常创建文件或打开文件
			   fp = fopen(path_, "w");
			   if (fp == NULL){
				   printf("无法另存到新文件,请检查磁盘目录权限问题..\n");
				   goto ins;
			   }
			   fclose(fp);	//检查完成
			   colour_str = (char*)malloc(nFileLen);
			   memset(colour_str, 0, nFileLen);
			   for (int i = 0; i <= colour_->colour_size; ++i){
				   colour_ = colour_->colour_next;
				   sprintf(colour_str, "%s<colour:%s,MIN:R=%d&G=%d&B=%d&,MIX:R=%d&G=%d&B=%d&>", colour_str, colour_->colour_name, colour_->colour_min_rgb[0], colour_->colour_min_rgb[1], colour_->colour_min_rgb[2], colour_->colour_mix_rgb[0], colour_->colour_mix_rgb[1], colour_->colour_mix_rgb[2]);
			   }
			   for (int j = 0; j < colour_->colour_size; ++j){	//恢复结点
				   colour_ = colour_->colour_top;
			   }
			   colour_features_seve(colour_str, path_);
			   printf("已另存到文件：%s,已自动检查文件是否有后缀，如果没有已经自动为其加上后缀名！\n", path_);
			   goto ins;

	
	}

	}




}

//视频识别器额外功能_图像操作
//去除图像锯齿-模糊平滑锯齿边缘
COLOURIN(void) image_Remove_sawtooth(IplImage **image/*模糊的图片*/, int Fuzzy_method = COLOUR_FUZZY_METHOD_GAUSSIAN/*模糊方法*/,int pox1 = 3/*卷积核*/,int pox2 = 3/*卷积核*/){	//卷积核越大平滑效果越强，速度越慢！模糊程度取像素点的3x3均值
	switch (Fuzzy_method){
	case COLOUR_FUZZY_METHOD_GAUSSIAN:	//高斯平滑
		cvSmooth(*image, *image, CV_GAUSSIAN, pox1, pox2);
		break;
	case COLOUR_FUZZY_METHOD_SIMPLE:	//简单平滑,若使用此平滑方式，pox2则参数无效，pox1参数则中值，也就是所有像素点的平滑程度/模糊，平滑值越高，平滑程度越高
		cvSmooth(*image, *image, CV_BLUR, pox1);
		break;
	case COLOUR_FUZZY_METHOD_MEDIAN:		//中值过滤，图像处理中一种保护边缘ROI区域不被平滑的处理方式，可以防止轮廓区域不被模糊化，防止特征减少！若使用此平滑方式，pox2则参数无效，pox1参数则为平滑程度
		cvSmooth(*image, *image, CV_MEDIAN, pox1);
		break;
	default:	//没有找到对应的方法，打印输出
		colour_error_type.colour_sign = COLOUR_BOOL_TRUE;
		strcpy_s(colour_error_type.error_type, COLOUR_ERROR__NOT_NO_METHOD);
		return;
	break;
	}
}


int main()
{
	//将特征库保存一份
	/*
	char str[] = { "<colour:红色,MIN:R=0&G=0&B=0&,MIX:R=255&G=0&B=0&><colour:绿色,MIN:R=0&G=0&B=0&,MIX:R=0&G=255&B=0&><colour:蓝色,MIN:R=0&G=0&B=0&,MIX:R=0&G=0&B=255&>" };
	char ss[256] = "d:\\1.x";
	//colour_features_seve(str, ss);
	*/
	/*
	//根据特征库对比颜色
	//声明一个颜色选择器
	colour* c = NULL;
	//分配内存
	colour_malloc(&c);
	//设置颜色特征
	char str[] = { "<colour:红色,MIN:R=0&G=0&B=0&,MIX:R=255&G=0&B=0&><colour:绿色,MIN:R=0&G=0&B=0&,MIX:R=0&G=255&B=0&><colour:蓝色,MIN:R=0&G=0&B=0&,MIX:R=0&G=0&B=255&>" };
	char * p_str = str;
	colour_range_control(&c, &p_str, COLOUR_BOOL_FALSE);
	if (colour_error_type.colour_sign == COLOUR_BOOL_TRUE){
	printf("colour_type_error:%s", colour_error_type.error_type);
	getchar();
	return -1;
	}
	*/
	/*
	//对比颜色
	image_colour_form *image_colour_form_return;
	image_form_malloc(&image_colour_form_return);
	IplImage* img = cvLoadImage("e:\\1.png");
	if (img == NULL){
	printf("无法打开图像文件!");
	getchar();
	return -1;
	};
	char str[] = { "<colour:红色,MIN:R=0&G=0&B=0&,MIX:R=255&G=0&B=0&><colour:红色,MIN:R=0&G=0&B=0&,MIX:R=255&G=0&B=0&><colour:红色,MIN:R=0&G=0&B=0&,MIX:R=255&G=0&B=0&>" };
	char * p_str = str;
	colour* c = NULL;
	//分配内存
	colour_malloc(&c);
	colour_range_control(&c, &p_str, COLOUR_BOOL_FALSE);
	image_colour_form_return = Colour_Distinguish(img, &c);
	printf("%s", image_colour_form_return->colour_name);
	getchar();
	*/
	/*
	//手动颜色训练
	char str[256] = "d:\\1.colour";
	colour_manual_generate(str);
	if (colour_error_type.colour_sign == COLOUR_BOOL_TRUE){
	printf("colour_type_error:%s", colour_error_type.error_type);
	getchar();
	}
	printf("手动，训练完成...");
	getchar();
	*/
	//颜色特征库编辑

	getchar();
	return 0;
}










