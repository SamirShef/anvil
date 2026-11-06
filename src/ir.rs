use std::collections::HashMap;

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct InstId(pub usize);

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct BasicBlockId(pub usize);

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct FunctionId(pub usize);

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum BinaryKind {
    Add,
    Sub,
    Mul,
    Div,
    Rem,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum Inst {
    Binary {
        kind: BinaryKind,
        left: InstId,
        right: InstId,
    },
    Call {
        callee: FunctionId,
        args: Vec<InstId>,
    },
    Jump {
        bb: BasicBlockId,
    },
    JumpIf {
        cond: InstId,
        then_bb: BasicBlockId,
        else_bb: BasicBlockId,
    },
    Alloca,
    Load {
        ptr: InstId,
    },
    Store {
        ptr: InstId,
    },
}

pub struct BasicBlock {
    pub body: Vec<Inst>,
}

impl BasicBlock {
    pub fn new() -> Self {
        Self { body: Vec::new() }
    }

    pub fn get_inst(&self, idx: InstId) -> Option<&Inst> {
        self.body.get(idx.0)
    }

    pub fn insert_inst(&mut self, inst: Inst) -> Option<InstId> {
        self.body.push(inst);
        Some(InstId(self.body.len() - 1))
    }

    pub fn insert_inst_after(&mut self, after: InstId, inst: Inst) -> Option<InstId> {
        if self.body.is_empty() {
            self.body.push(inst);
            return Some(InstId(self.body.len() - 1));
        }
        if after.0 + 1 >= self.body.len() {
            return self.insert_inst(inst);
        }

        self.body.insert(after.0 + 1, inst);
        Some(InstId(after.0 + 1))
    }

    pub fn insert_inst_before(&mut self, before: InstId, inst: Inst) -> Option<InstId> {
        if before.0 >= self.body.len() || self.body.is_empty() {
            self.body.push(inst);
            return Some(InstId(self.body.len() - 1));
        }
        if before.0 == 0 {
            self.body.insert(0, inst);
            return Some(InstId(0));
        }

        self.body.insert(before.0, inst);
        Some(InstId(before.0 + 1))
    }
}

pub struct Function {
    pub body: Vec<BasicBlock>,
}

impl Function {
    pub fn new() -> Self {
        Self { body: Vec::new() }
    }

    pub fn get_bb(&self, idx: BasicBlockId) -> Option<&BasicBlock> {
        self.body.get(idx.0)
    }

    pub fn insert_inst(&mut self, idx: BasicBlockId, inst: Inst) -> bool {
        let bb = self.body.get_mut(idx.0);
        if bb.is_none() {
            return false;
        }

        bb.unwrap().insert_inst(inst);
        true
    }

    pub fn insert_inst_after(&mut self, idx: BasicBlockId, after: InstId, inst: Inst) -> bool {
        let bb = self.body.get_mut(idx.0);
        if bb.is_none() {
            return false;
        }

        bb.unwrap().insert_inst_after(after, inst);
        true
    }

    pub fn insert_inst_before(&mut self, idx: BasicBlockId, before: InstId, inst: Inst) -> bool {
        let bb = self.body.get_mut(idx.0);
        if bb.is_none() {
            return false;
        }

        bb.unwrap().insert_inst_before(before, inst);
        true
    }
}

pub struct Context {
    funcs: Vec<Function>,

    inst_names: HashMap<InstId, String>,
    bb_names: HashMap<(FunctionId, BasicBlockId), String>,
    func_names: HashMap<FunctionId, String>,
}

impl Context {
    pub fn new() -> Self {
        Self {
            funcs: Vec::new(),
            inst_names: HashMap::new(),
            bb_names: HashMap::new(),
            func_names: HashMap::new(),
        }
    }

    pub fn get_func(&self, func_id: FunctionId) -> Option<&Function> {
        self.funcs.get(func_id.0)
    }

    pub fn get_func_mut(&mut self, func_id: FunctionId) -> Option<&mut Function> {
        self.funcs.get_mut(func_id.0)
    }

    pub fn create_bb(&mut self, func_id: FunctionId) -> Option<BasicBlockId> {
        self.create_bb_impl(func_id)
    }

    pub fn create_bb_with_name(
        &mut self,
        func_id: FunctionId,
        name: impl Into<String>,
    ) -> Option<BasicBlockId> {
        let id = self.create_bb_impl(func_id);
        self.bb_names.insert((func_id, id.unwrap()), name.into());

        id
    }

    fn create_bb_impl(&mut self, func_id: FunctionId) -> Option<BasicBlockId> {
        let idx = func_id.0;
        let id = BasicBlockId(self.funcs[idx].body.len());
        self.funcs[idx].body.push(BasicBlock::new());

        Some(id)
    }

    pub fn create_function(&mut self) -> Option<FunctionId> {
        self.create_function_impl()
    }

    pub fn create_function_with_name(&mut self, name: impl Into<String>) -> Option<FunctionId> {
        let id = self.create_function_impl();
        id?;

        self.func_names.insert(id.unwrap(), name.into());
        id
    }

    fn create_function_impl(&mut self) -> Option<FunctionId> {
        let id = FunctionId(self.funcs.len());
        self.funcs.push(Function::new());

        Some(id)
    }
}

pub struct IRBuilder {
    cur_func: Option<FunctionId>,
    cur_bb: Option<BasicBlockId>,
    cur_unnamed_func_idx: usize,
    cur_unnamed_local_idx: usize,
}

impl IRBuilder {
    pub fn new() -> Self {
        Self {
            cur_func: None,
            cur_bb: None,
            cur_unnamed_func_idx: 0,
            cur_unnamed_local_idx: 0,
        }
    }

    pub fn set_insert_point(&mut self, func: FunctionId, bb: BasicBlockId) {
        self.cur_func = Some(func);
        self.cur_bb = Some(bb);
    }

    pub fn create_function(&mut self, ctx: &mut Context) -> Option<FunctionId> {
        let name = self.cur_unnamed_func_idx.to_string();
        self.create_function_with_name(ctx, name)
    }

    pub fn create_function_with_name(
        &mut self,
        ctx: &mut Context,
        name: impl Into<String>,
    ) -> Option<FunctionId> {
        ctx.create_function_with_name(name.into())
    }

    pub fn create_bb(&mut self, ctx: &mut Context, func_id: FunctionId) -> Option<BasicBlockId> {
        let name = self.cur_unnamed_func_idx.to_string();
        self.create_bb_with_name(ctx, func_id, name)
    }

    pub fn create_bb_with_name(
        &mut self,
        ctx: &mut Context,
        func_id: FunctionId,
        name: impl Into<String>,
    ) -> Option<BasicBlockId> {
        ctx.create_bb_with_name(func_id, name.into())
    }

    fn emit_inst(
        &mut self,
        ctx: &mut Context,
        inst: Inst,
        name: impl Into<String>,
    ) -> Option<InstId> {
        self.cur_bb?;

        let bb = ctx.funcs[self.cur_func.unwrap().0]
            .body
            .get_mut(self.cur_bb.unwrap().0)
            .unwrap();
        let id = bb.insert_inst(inst);
        ctx.inst_names.insert(id.unwrap(), name.into());

        id
    }

    pub fn emit_add(&mut self, ctx: &mut Context, left: InstId, right: InstId) -> Option<InstId> {
        let name = self.cur_unnamed_local_idx.to_string();
        self.emit_add_with_name(ctx, left, right, name)
    }

    pub fn emit_add_with_name(
        &mut self,
        ctx: &mut Context,
        left: InstId,
        right: InstId,
        name: impl Into<String>,
    ) -> Option<InstId> {
        self.emit_inst(
            ctx,
            Inst::Binary {
                kind: BinaryKind::Add,
                left,
                right,
            },
            name,
        )
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn create_function() {
        let mut context = Context::new();
        context.create_function();

        assert_eq!(context.funcs.len(), 1);
        assert!(context.func_names.is_empty());

        context.create_function_with_name("main");

        assert_eq!(context.funcs.len(), 2);
        assert_eq!(context.func_names.len(), 1);
        assert_eq!(context.func_names.get(&FunctionId(1)).unwrap(), "main");
    }

    #[test]
    fn create_basic_block() {
        let mut context = Context::new();
        let func_id = context.create_function();

        assert_eq!(context.funcs.len(), 1);
        assert!(context.func_names.is_empty());

        let func_id = func_id.unwrap();
        context.create_bb(func_id);

        assert_eq!(context.funcs[func_id.0].body.len(), 1);
        assert!(context.bb_names.is_empty());

        context.create_bb_with_name(func_id, "entry");

        assert_eq!(context.funcs[func_id.0].body.len(), 2);
        assert_eq!(context.bb_names.len(), 1);
        assert_eq!(
            context.bb_names.get(&(func_id, BasicBlockId(1))).unwrap(),
            "entry"
        );
    }

    #[test]
    fn create_instruction() {
        let mut context = Context::new();
        let func_id = context.create_function_with_name("main");

        assert_eq!(context.funcs.len(), 1);
        assert_eq!(context.func_names.len(), 1);
        assert_eq!(context.func_names.get(&FunctionId(0)).unwrap(), "main");

        let func_id = func_id.unwrap();
        context.create_bb_with_name(func_id, "entry");

        assert_eq!(context.funcs[func_id.0].body.len(), 1);
        assert_eq!(context.bb_names.len(), 1);
        assert_eq!(
            context.bb_names.get(&(func_id, BasicBlockId(0))).unwrap(),
            "entry"
        );

        context
            .get_func_mut(func_id)
            .unwrap()
            .insert_inst(BasicBlockId(0), Inst::Alloca);

        assert_eq!(context.funcs[func_id.0].body[0].body.len(), 1);
        assert_eq!(context.funcs[func_id.0].body[0].body[0], Inst::Alloca);

        context.get_func_mut(func_id).unwrap().insert_inst_after(
            BasicBlockId(0),
            InstId(0),
            Inst::Call {
                callee: FunctionId(0),
                args: Vec::new(),
            },
        );

        assert_eq!(context.funcs[func_id.0].body[0].body.len(), 2);
        assert_eq!(context.funcs[func_id.0].body[0].body[0], Inst::Alloca);
        assert_eq!(
            context.funcs[func_id.0].body[0].body[1],
            Inst::Call {
                callee: FunctionId(0),
                args: Vec::new(),
            }
        );

        context.get_func_mut(func_id).unwrap().insert_inst_before(
            BasicBlockId(0),
            InstId(1),
            Inst::Load { ptr: InstId(0) },
        );

        assert_eq!(context.funcs[func_id.0].body[0].body.len(), 3);
        assert_eq!(context.funcs[func_id.0].body[0].body[0], Inst::Alloca);
        assert_eq!(
            context.funcs[func_id.0].body[0].body[1],
            Inst::Load { ptr: InstId(0) }
        );
        assert_eq!(
            context.funcs[func_id.0].body[0].body[2],
            Inst::Call {
                callee: FunctionId(0),
                args: Vec::new(),
            }
        );
    }
}
