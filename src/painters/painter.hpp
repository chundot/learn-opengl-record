class Painter {
 public:
  static Painter *cur;
  virtual void init() {}
  virtual void terminate() {}
  virtual void onRender() {}
};