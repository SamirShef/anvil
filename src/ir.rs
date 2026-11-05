use std::collections::HashMap;

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct InstId(pub usize);

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct BasicBlockId(pub usize);

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct FunctionId(pub usize);

pub enum Inst {
    Binary(InstId, InstId),
    Call(FunctionId),
    Jump(BasicBlockId),
    JumpIf(InstId, BasicBlockId, BasicBlockId),
    Alloca,
    Load(InstId),
    Store(InstId),
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

    pub fn insert_inst(&mut self, inst: Inst) {
        self.body.push(inst);
    }

    pub fn insert_inst_after(&mut self, after: InstId, inst: Inst) {
        if after.0 >= self.body.len() {
            return self.insert_inst(inst);
        }

        self.body.insert(after.0, inst);
    }

    pub fn insert_inst_before(&mut self, before: InstId, inst: Inst) {
        if before.0 == 0 {
            return self.body.insert(0, inst);
        }

        self.body.insert(before.0 - 1, inst);
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
    bb_names: HashMap<BasicBlockId, String>,
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

    pub fn create_bb(&mut self, func_id: FunctionId) -> Option<BasicBlockId> {
        self.create_bb_with_name(func_id, "")
    }

    pub fn create_bb_with_name(
        &mut self,
        func_id: FunctionId,
        name: impl Into<String>,
    ) -> Option<BasicBlockId> {
        let idx = func_id.0;
        let id = BasicBlockId(self.funcs[idx].body.len());
        self.funcs[idx].body.push(BasicBlock::new());

        self.bb_names.insert(id, name.into().clone());

        Some(id)
    }

    pub fn create_function(&mut self) -> Option<FunctionId> {
        self.create_function_with_name("")
    }

    pub fn create_function_with_name(&mut self, name: impl Into<String>) -> Option<FunctionId> {
        let id = FunctionId(self.funcs.len());
        self.funcs.push(Function::new());

        self.func_names.insert(id, name.into().clone());

        Some(id)
    }
}
