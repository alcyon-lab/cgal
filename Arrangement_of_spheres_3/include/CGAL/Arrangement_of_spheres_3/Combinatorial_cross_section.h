#ifndef CGAL_ARRANGEMENT_OF_SPHERES_CROSS_SECTION_H
#define CGAL_ARRANGEMENT_OF_SPHERES_CROSS_SECTION_H
#include <CGAL/Kinetic/basic.h>
#include <CGAL/HalfedgeDS_default.h>
#include <CGAL/HalfedgeDS_vertex_base.h>
#include <CGAL/HalfedgeDS_halfedge_base.h>
#include <CGAL/HalfedgeDS_decorator.h>
#include <CGAL/HalfedgeDS_face_base.h>
#include <CGAL/Arrangement_of_spheres_3/Rule_direction.h>

#include <CGAL/Arrangement_of_spheres_3/Combinatorial_vertex.h>
#include <CGAL/Arrangement_of_spheres_3/Combinatorial_curve.h>
#include <CGAL/Arrangement_of_spheres_3_basic.h>
#include <boost/utility.hpp>
#include <map>
#include <set>
//#include <boost/array.hpp>
#include <boost/foreach.hpp>

CGAL_AOS3_BEGIN_INTERNAL_NAMESPACE

CGAL_AOS3_TEMPLATE
class Cross_section_initializer;

CGAL_AOS3_TEMPLATE
class Combinatorial_cross_section: public boost::noncopyable {
public:
  CGAL_AOS3_TRAITS;

  friend class Cross_section_initializer CGAL_AOS3_TARG;

  /*
    Use an hds. For each halfedge mark what sphere, what part and if
    part of the circle, whether inside or outside

    For each edge we also might have an event
  */

  struct Handle_compare{
    template <class H>
    bool operator()(H a, H b) const {
      return &*a < &*b;
    }
  };
public:
  typedef Combinatorial_vertex Point;
  typedef Combinatorial_curve Curve;
  typedef CGAL_AOS3_TYPENAME CGAL_AOS3_INTERNAL_NS::Rule_direction Rule_direction;
 
  typedef CGAL_AOS3_TYPENAME Traits::Event_key Event_key;

  struct Slice_halfedgeDS_items_2 {
    template < class Refs, class Traits>
    struct Vertex_wrapper {
     
      struct Vertex: public CGAL::HalfedgeDS_vertex_base< Refs, CGAL::Tag_true, Point>{
      };
    };
    template < class Refs, class Traits>
    struct Halfedge_wrapper {
      struct Halfedge: public CGAL::HalfedgeDS_halfedge_base< Refs> {
	Halfedge(){}
	Curve curve() const {
	  return pt_;
	}
	Curve& curve() {
	  return pt_;
	}
	void set_curve(Curve pt) {
	  pt_=pt;
	}
	Event_key event() const {
	  return ev_;
	}
	void set_event(Event_key ev) {
	  ev_=ev;
	}

	Event_key ev_;
	Curve pt_;
      };
    };
    template < class Refs, class Traits>
    struct Face_wrapper {
      // maybe want pointers to chains, maybe not
      typedef CGAL::HalfedgeDS_face_base< Refs> Face;
    };
  };
  

    

  typedef CGAL::HalfedgeDS_default<int, Slice_halfedgeDS_items_2> HDS;

  typedef CGAL_AOS3_TYPENAME HDS::Halfedge_handle Halfedge_handle;
  typedef CGAL_AOS3_TYPENAME HDS::Halfedge_const_handle Halfedge_const_handle;
  typedef CGAL_AOS3_TYPENAME HDS::Vertex_handle Vertex_handle;
  typedef CGAL_AOS3_TYPENAME HDS::Vertex_const_handle Vertex_const_handle;
  typedef CGAL_AOS3_TYPENAME HDS::Face_handle Face_handle;
  typedef CGAL_AOS3_TYPENAME HDS::Face_const_handle Face_const_handle;

  Combinatorial_cross_section();

  HDS& hds();

  struct HE_key: public HDS::Halfedge_const_handle {
    typedef CGAL_AOS3_TYPENAME HDS::Halfedge_const_handle P;
    HE_key(P h): P(h){}
    bool operator<(P o) const {
      return &P::operator*() < &o.operator*();
    }
  };



  CGAL_CONST_ITERATOR(Halfedge, halfedge, CGAL_AOS3_TYPENAME HDS::Halfedge_const_iterator,
		      return hds_.halfedges_begin(),
		      return hds_.halfedges_end());
  
  
  CGAL_CONST_ITERATOR(Vertex, vertice, CGAL_AOS3_TYPENAME HDS::Vertex_const_iterator,
		      return hds_.vertices_begin(),
		      return hds_.vertices_end());
  
  CGAL_CONST_ITERATOR(Face, face, CGAL_AOS3_TYPENAME HDS::Face_const_iterator,
		      return hds_.faces_begin(),
		      return hds_.faces_end());
  
  CGAL_SIZE(vertices, return hds_.size_of_vertices();)

  CGAL_SIZE(faces, return hds_.size_of_faces();)
  CGAL_SIZE(edges, return hds_.size_of_halfedges();)

  Halfedge_handle find_halfedge(Vertex_handle v, Face_handle f);

  bool has_vertex(Face_const_handle fh, Vertex_const_handle vh) const;

  std::ostream &write(Halfedge_const_handle h, std::ostream &out) const;
  std::ostream &write(Vertex_const_handle h, std::ostream &out) const;

  std::ostream &write(Face_const_handle h, std::ostream &out) const;

  std::ostream &write(std::ostream &out) const;

  bool is_in_slice(Vertex_const_handle v) const;
  bool is_in_slice(Halfedge_const_handle h) const;
  bool is_in_slice(Face_const_handle h) const;

  unsigned int degree(Vertex_const_handle v) const;

  bool is_redundant(Vertex_const_handle v) const;

  void clear();

  CGAL_GETNR(Face_handle, infinite_face, return inf_);
  //Face_handle infinite_face(){return inf_;}

  CGAL_ITERATOR(Halfedge, halfedge, CGAL_AOS3_TYPENAME HDS::Halfedge_iterator,
		return hds_.halfedges_begin(),
		return hds_.halfedges_end());

  CGAL_ITERATOR(Face, face,CGAL_AOS3_TYPENAME HDS::Face_iterator,
		return hds_.faces_begin(),
		return hds_.faces_end());

  
  // the first is the target of h, second the source
  std::pair<Halfedge_handle, Halfedge_handle> remove_edge(Halfedge_handle h);

  void set_curve(Halfedge_handle h, Curve c);
   
  /*void set_event(Halfedge_handle h, Event_key k) const {
    CGAL_assertion(k != Event_key());
    CGAL_assertion(h->event()== h->opposite()->event());
    CGAL_assertion(h->event() == Event_key());
    h->set_event(k);
    h->opposite()->set_event(k);
  }

  void unset_event(Halfedge_handle h) const {
    CGAL_assertion(h->event()== h->opposite()->event());
    h->set_event(Event_key());
    h->opposite()->set_event(Event_key());
    }*/

  Face_handle merge_faces(Halfedge_handle e);

  Face_handle merge_faces(Vertex_handle v);
  
  Halfedge_handle split_face(Curve c, Halfedge_handle source,
			     Halfedge_handle target);
  
  /*void insert_target(Curve::Key k,
		     Halfedge_handle cps[]);

  Face_handle remove_target(Halfedge_handle ts[],
  Halfedge_handle vert[]);*/
  /*// move old_edge to have the vertices be in new_source and new_target
    // return the new edge
    Halfedge_handle move_edge(Halfedge_handle old_edge,
    Halfedge_handle new_source,
    Halfedge_handle new_target,
    Halfedge_handle &blank_old_source,
    Halfedge_handle &blank_old_target);*/
	
  // remove the rule from the endpoint
  // attach it to the new one
  void move_edge_target(Halfedge_handle edge,
			Halfedge_handle new_target); 

 

  //! stitch the the HDS together by attaching the vertices pointed to by ib:ie to those in fb,fe
  /*!
    New edges are stuck in edges
   */
  template <class It, class Cit>
  void stitch_in(It ib, It ie, It fb, Cit cb) {
    CGAL_precondition(num_components_ > 1);
    std::cout << "Stitching: " << std::endl;
    Cit cc=cb;
    for (It ic= ib, fc= fb; ic != ie; ++ic, ++fc, ++cc) {
      write(*ic, std::cout);
      std::cout << " to ";
      write(*fc, std::cout);
      std::cout << " with " << *cc << std::endl;
    }

    --num_components_;
    Face_handle f= (*fb)->face();
    for (It ic=ib, fc= fb; ic != ie; ++ic, ++fc) {
      CGAL_assertion((*ic)->face() == inf_);
      CGAL_assertion((*fc)->face() == (*fb)->face());
    }
   
    Halfedge_handle c= *ib;
    do {
      CGAL_assertion(c->face() == inf_);
      c->set_face(f);
      c=c->next();
    } while (c != *ib);
    
    Halfedge_handle hn= new_halfedge(*cb);
    //*edges= hn;
    //++edges;
    hn->set_face(f);
    hn->opposite()->set_face(f);
    hn->set_vertex((*fb)->vertex());
    hn->opposite()->set_vertex((*ib)->vertex());
    connect( hn, (*fb)->next());
    connect(*fb, hn->opposite());
    connect(hn->opposite(), (*ib)->next());
    connect(*ib, hn);

    ++ib; ++fb; ++cb;
    CGAL::HalfedgeDS_decorator<HDS> hdsd(hds_);
    for (; ib != ie; ++ib, ++fb, ++cb) {
      Halfedge_handle h= hdsd.split_face(*ib, *fb);
      h->set_curve(*cb);
      h->opposite()->set_curve(cb->other_side());
      // *edges=h;
      //++edges;
    }
    audit();
  }

  template <class It>
  Face_handle snip_out(It b, It e) {
    std::cout << "Snipping out ";
    for (It c= b; c != e; ++c) {
      write(*c, std::cout) << ", ";
    }
    std::cout << std::endl;
    ++num_components_;
    It c=b;
    ++c; ++c;
    CGAL::HalfedgeDS_decorator<HDS> hdsd(hds_);
    for (; c!= e; ++c) {
      hdsd.join_face(*c);
    }
    c=b;
    Halfedge_handle h1= *c;
    ++c;
    Halfedge_handle h2= *c;

    Halfedge_handle inside, outside;
    Face_handle f= h1->face();
    Face_handle of= h1->opposite()->face();

    if (h1->prev() != h2->opposite()) {
      inside = h1->prev();
    } else if (h2->prev() != h1->opposite()) {
      inside= h2->prev();
    }
    if (h1->next() != h2->opposite()) {
      outside= h1->next();
    } else if (h2->next() != h1->opposite()) {
      outside= h2->next();
    }

    connect(h1->opposite()->prev(), h1->next());
    connect(h2->opposite()->prev(), h2->next());
 
    connect(h1->prev(), h1->opposite()->next());
    connect(h2->prev(), h2->opposite()->next());
   
    CGAL_assertion(outside != Halfedge_handle());
    Halfedge_handle hc= outside;
    do {
      hc->vertex()->set_halfedge(hc);
      hc->set_face(f);
      hc=hc->next();
    } while (hc != h1->next());
    f->set_halfedge(outside);


    if (inside != Halfedge_handle()) {
      Halfedge_handle hc= inside;
      do {
	hc->set_face(inf_);
	hc->vertex()->set_halfedge(hc);
	hc=hc->next();
      } while (hc != inside);
    } else {
      h1->opposite()->vertex()->set_halfedge(Halfedge_handle());
    }
    hds_.faces_erase(of);
    delete_edge(h1);
    delete_edge(h2);

    write(std::cout);
    /* std::cout << "Auditing const decorator..." << std::flush;
    CGAL::HalfedgeDS_const_decorator<HDS> chds(hds_);
    if (!chds.is_valid(true, num_components_==1? 3: 2)) {
      CGAL_assertion(0);
      std::cerr << "Not valid." << std::endl;
    }
    std::cout << "done." << std::endl;*/

    return f;
  }

  void delete_component(Vertex_handle vh) {
    std::cout << "Deleting component from ";
    write(vh, std::cout) << std::endl;
    --num_components_;
    std::vector<Vertex_handle> stack;
    std::set<Vertex_handle, Handle_compare> vertices;
    std::set<Halfedge_handle, Handle_compare> edges;
    std::set<Face_handle, Handle_compare> faces;
    stack.push_back(vh);
    vertices.insert(vh);
    do {
      Vertex_handle vhc= stack.back();
      stack.pop_back();
      if (vhc->halfedge() != Halfedge_handle()) {
	Halfedge_handle h= vhc->halfedge()->opposite();
	do {
	  if (edges.find(h) == edges.end()
	      && edges.find(h->opposite()) == edges.end() ) {
	    edges.insert(h);
	  }
	  if (vertices.find(h->vertex()) == vertices.end()) {
	    vertices.insert(h->vertex());
	    stack.push_back(h->vertex());
	  }
	  if (faces.find(h->face()) == faces.end() && h->face() != inf_) {
	    faces.insert(h->face());
	  }
	  h=h->opposite()->next();
	} while (h != vhc->halfedge()->opposite());
      }
    } while (!stack.empty());
    // hds_.vertices_erase(_1)
    std::cout << "Deleting: \n";
    BOOST_FOREACH(Vertex_handle v, vertices) {
      write(v, std::cout) << std::endl;
       hds_.vertices_erase(v);
    } 
    BOOST_FOREACH(Halfedge_handle h, edges){
     write(h, std::cout) << std::endl;
      hds_.edges_erase(h);
    }
    BOOST_FOREACH(Face_handle f, faces){
     write(f, std::cout) << std::endl;
      hds_.faces_erase(f);
    }
    write(std::cout);
    std::cout << "Auditing const decorator..." << std::flush;
    CGAL::HalfedgeDS_const_decorator<HDS> chds(hds_);
    if (!chds.is_valid(true, num_components_==1? 3: 2)) {
      CGAL_assertion(0);
      std::cerr << "Not valid." << std::endl;
    }
    std::cout << "done." << std::endl;
    
  }

  void delete_circle(Vertex_handle v) {
    halfedges_[v->point().key().input_index()]=Halfedge_handle();
    delete_component(v);
  }

  void audit(bool extra_vertices=false) const ;	 


  typedef std::pair<Point, Curve> NFP;

  void reserve(int nv, int ne, int nf);

 
  void set_has_boundary(bool tf);

  bool has_boundary() const;

  Halfedge_handle remove_vertex(Halfedge_handle v);

  void exchange_sphere_extremums(Curve::Key k, Curve::Key l);

  Halfedge_handle halfedge(Vertex_handle v, Face_handle f) const {
    Halfedge_handle h= v->halfedge();
    do {
      if (h->face() == f) return h;
      h= h->next()->opposite();
    } while(h != v->halfedge());
    CGAL_assertion(0);
    return Halfedge_handle();
  }

  Halfedge_handle next_edge_on_curve(Halfedge_handle) const;
  Halfedge_handle next_edge_on_rule(Halfedge_handle) const;

  // an outward facing edge
  Halfedge_handle cross_edge(Halfedge_handle) const;

  // a halfedge on the curve (an inside one)
  Halfedge_handle a_halfedge(Curve::Key k) const;

  // a halfedge on the rule pointing to the extremal vertex
  // Halfedge_handle rule_halfedge(Curve::Key k, Rule_direction i) const;

  // a halfedge on the circle pointing to the extremal vertex (inside)
  //Halfedge_handle extremum_halfedge(Curve::Key k, Rule_direction i) const;
  
  // insert the vertex so that h->opposite points to it
  Halfedge_handle insert_vertex( Point p, Halfedge_handle h);


  std::pair<Halfedge_handle,Halfedge_handle> 
  intersect(Halfedge_handle ha, Halfedge_handle hb);

  std::pair<Halfedge_handle,Halfedge_handle>  unintersect(Face_handle f);

 
  
  //friend class Cross_section_initializer CGAL_AOS3_TARG;
 

  void relabel_rule(Halfedge_handle h, Curve nl);

 

  //typedef boost::array<Halfedge_handle, 4> Halfedge_quad;

  /*const Halfedge_handle halfedges(CGAL_AOS3_TYPENAME Curve::Key k) const {
    return halfedges_[k.input_index()];
  }

   Halfedge_quad& halfedges(CGAL_AOS3_TYPENAME Curve::Key k)  {
    return halfedges_[k.input_index()];
    }*/
  void set_number_of_spheres(unsigned int i) {
    CGAL_assertion(halfedges_.size() <=i);
    halfedges_.resize(i);
  }

  void new_circle(CGAL_AOS3_TYPENAME Curve::Key k, Halfedge_handle c[]);

private:
  void connect(Halfedge_handle a, Halfedge_handle b);

  /*typedef boost::tuple<Halfedge_handle, Halfedge_handle,
    Halfedge_handle, Halfedge_handle> Halfedge_handle_quadruple;*/

  //void new_target(Curve::Key k, Halfedge_handle tar[]);
  
  void set_halfedge(Halfedge_handle h);


  /*Halfedge_handle split_face(Halfedge_handle o, Halfedge_handle d,
    Curve c);*/

  //void relabel_target(Halfedge_handle v[], Curve::Key k);

  
  //void exchange_vertices(Halfedge_handle h, Halfedge_handle p);

  void audit_vertex(Vertex_const_handle v, bool extra) const ;
  


  //void audit_halfedge(Halfedge_const_handle v) const ;





  void delete_edge(Halfedge_handle h);

  template <class Out>
  void find_halfedges(Curve c, Out o) const {
    for (Halfedge_const_iterator it= halfedges_begin();
	 it != halfedges_end(); ++it) {
      if (it->curve() == c) {
	*o=it;
	++o;
      }
    }
  }
  
  


  // insert the vertex so that h->opposite points to it
  Halfedge_handle insert_vertex_in_edge_unsafe(Halfedge_handle h,
					       Vertex_handle  vh);

  // insert a vertex for p in both edges
  // merge the two vertices
  std::pair<Halfedge_handle, Halfedge_handle> pinch_bl(Halfedge_handle a,
						       Halfedge_handle b, Point p);

  // a and b go to the same vertex. Make them go to different vertices
  // return the new vertex on b
  Vertex_handle unpinch_bl(Halfedge_handle a, Halfedge_handle b);

  void merge_vertices_bl(Halfedge_handle a, Halfedge_handle b);

  Halfedge_handle new_halfedge(Curve c);

  //typedef std::pair<Point,Point>  ED;

  Vertex_handle new_vertex(Point p);

 

 
  mutable HDS hds_;
  Face_handle inf_;
  mutable std::vector<Curve> errors_;
  //std::vector<Vertex_handle> targets_;
  std::vector<Halfedge_handle> halfedges_;
  int num_components_;

  // for new_face when constructing things
  //std::map<Edge, Halfedge_handle> unmatched_hedges_;
  //std::map<Point, Vertex_handle> points_;
    
};

CGAL_AOS3_END_INTERNAL_NAMESPACE

#ifdef CGAL_AOS3_USE_TEMPLATES
#include <CGAL/Arrangement_of_spheres_3/Combinatorial_cross_section_impl.h>
#endif

#endif
