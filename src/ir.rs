pub type InstId = usize;

pub enum InstData {
    Binary(InstId, InstId),
    Call(Function),
    Jump(BasicBlock),
    JumpIf(InstId, BasicBlock, BasicBlock),
    Alloca,
    Load(InstId),
    Store(InstId),
}

pub struct Inst {
    pub name: String,
    pub data: InstData,
}

pub struct BasicBlock {
    pub name: String,
    pub body: Vec<Inst>,
}

pub struct Function {
    pub name: String,
    pub body: Vec<BasicBlock>,
}
