// CppTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <memory>
#include <vector>

//class Sphere
//{
//public:
//    Sphere(float r) : radius(r) {}
//    void show() { printf("Radius: %f\n", radius); }
//public:
//    float radius = 0.0f;
//};
//
//
//void show_sphere(Sphere s)
//{
//    s.show();
//}
//
//int main()
//{
//    auto s = std::make_shared<Sphere>(10);
//
//    show_sphere(*s);
//}

//################################################################################

//class A
//{
//public:
//    A() { printf("Constructor\n");  }
//    ~A() { printf("Destructor\n");  }
//
//};
//
//int main()
//{
//    std::shared_ptr<A> p(new A);
//    std::vector<std::shared_ptr<A>> v;
//
//    v.push_back(std::move(p));
//
//
//
//    printf("Before clear\n");
//    v.clear();
//    printf("After clear\n");
//    
//}

//#################################################################################

//class A
//{
//public:
//    A() { printf("Constructor\n");  }
//    ~A() { printf("Destructor\n");  }
//
//};
//
//
//std::shared_ptr<A> really_create_a()
//{
//    auto p = std::make_shared<A>();
//    printf("Returning from really_create_a()\n");
//    return p;
//}
//
//
//std::shared_ptr<A> create_a()
//{
//    auto p = really_create_a();
//    printf("Returning from create_a() %d\n", p.use_count());
//    return p;
//}
//
//void use_a(std::shared_ptr<A> p)
//{
//    printf("Inside use_a(): ref count = %d\n", p.use_count());
//}
//
//int main()
//{
//    auto p = create_a();
//    use_a(p);
//    printf("Returned from main()\n");
//    return 0;
//}

//#################################################################################


//class A
//{
//public:
//    void advance()
//    {
//        printf("A advancing\n");
//    }
//
//    A() { printf("Constructor\n"); }
//    ~A() { printf("Destructor\n"); }
//
//};
//
//class Container
//{
//public:
//
//    void do_work()
//    {
//        for (std::shared_ptr<A> p : all_a)
//        {
//            p->advance();
//        }
//    }
//
//    void add_a(std::shared_ptr<A> p)
//    {
//        all_a.push_back(p);
//    }
//
//    void remove_a()
//    {
//        std::shared_ptr<A> p = *all_a.begin();
//        all_a.pop_back();
//    }
//
//    std::vector<std::shared_ptr<A>> all_a;
//};
//
//void init(Container * c)
//{
//    auto p = std::make_shared<A>();
//    c->add_a(p);
//
//}
//
//int main()
//{
//    Container c;
//    init(&c);
//
//    c.do_work();
//    c.do_work();
//    printf("All work done\n");
//    c.remove_a();
//    printf("Remove done\n");
//
//}


struct Sphere
{
	Sphere() {}

	int _radius;
	void putprop(int r) {
		_radius = r;
		printf("Setting radius to %d\n", _radius);
	}
	int getprop() {
		printf("Getting radius %d\n", _radius);
		return _radius;
	}

	__declspec(property(get = getprop, put = putprop)) int radius;
};

#include <string.h>

int main()
{
	Sphere s;
	s.radius = 10;
	printf("Radius is %d\n", s.radius);

	//int& refRadius = s.radius;
	//int* pRadius = &s.radius;
}